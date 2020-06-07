/****************************************
 *
 *   INSERT-PROJECT-NAME-HERE - INSERT-GENERIC-NAME-HERE
 *   Copyright (C) 2020 Victor Tran
 *
 *   This program is free software: you can redistribute it and/or modify
 *   it under the terms of the GNU General Public License as published by
 *   the Free Software Foundation, either version 3 of the License, or
 *   (at your option) any later version.
 *
 *   This program is distributed in the hope that it will be useful,
 *   but WITHOUT ANY WARRANTY; without even the implied warranty of
 *   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *   GNU General Public License for more details.
 *
 *   You should have received a copy of the GNU General Public License
 *   along with this program.  If not, see <http://www.gnu.org/licenses/>.
 *
 * *************************************/
#include "timer.h"

#include <QSqlQuery>
#include <QVariant>
#include <QDBusConnection>
#include <tnotification.h>

#include "timer_adaptor.h"

struct TimerPrivate {
    enum State {
        Running,
        Paused,
        Stopped
    };

    int id;
    State state;

    quint64 length;
    quint64 timeoutDate;
    quint64 pauseLengthRemain = 0;

    QString note;

    QTimer* timer;
};

Timer::Timer(int id, QObject* parent) : QObject(parent) {
    d = new TimerPrivate();
    d->id = id;

    d->timer = new QTimer(this);
    d->timer->setSingleShot(true);
    connect(d->timer, &QTimer::timeout, this, &Timer::doTimeout);

    updateFromDatabase();

    new TimerAdaptor(this);
    QDBusConnection::sessionBus().registerObject(this->objectPath(), this);
}

Timer::~Timer() {
    emit Removing();

    delete d;
}

QString Timer::objectPath() {
    return QStringLiteral("/com/vicr123/the24/Timers/%1").arg(d->id);
}

void Timer::Remove() {
    QSqlQuery query;
    query.prepare("DELETE FROM timers WHERE id=:id");
    query.bindValue(":id", d->id);
    query.exec();

    this->deleteLater();
}

qlonglong Timer::MillisecondsRemaining() {
    if (d->state == TimerPrivate::Running) { //Running
        return d->timer->remainingTime();
    } else if (d->state == TimerPrivate::Paused) { //Paused
        return d->pauseLengthRemain;
    } else { //Stopped
        return d->length;
    }
}

void Timer::SetLength(quint64 length) {
    if (d->state != TimerPrivate::Stopped) return;

    d->length = length;
    emit LengthChanged();
    writeToDatabase();
}

void Timer::SetNote(QString note) {
    d->note = note;
    emit NoteChanged();
    writeToDatabase();
}

void Timer::Pause() {
    if (d->state != TimerPrivate::Running) return;

    d->state = TimerPrivate::Paused;
    d->pauseLengthRemain = d->timer->remainingTime();
    d->timer->stop();

    emit StateChanged();
    writeToDatabase();
}

void Timer::Resume() {
    if (d->state == TimerPrivate::Paused) { //Paused
        d->timeoutDate = QDateTime::currentMSecsSinceEpoch() + d->pauseLengthRemain;
    } else { //Stopped
        d->timeoutDate = QDateTime::currentMSecsSinceEpoch() + d->length;
    }
    d->state = TimerPrivate::Running;

    d->timer->setInterval(QDateTime::currentDateTimeUtc().msecsTo(QDateTime::fromMSecsSinceEpoch(d->timeoutDate)));
    d->timer->start();

    emit StateChanged();
    writeToDatabase();
}

void Timer::Reset() {
    d->timer->stop();
    d->state = TimerPrivate::Stopped;

    emit StateChanged();
    writeToDatabase();
}

QString Timer::State() {
    switch (d->state) {
        case TimerPrivate::Running:
            return "Running";
        case TimerPrivate::Paused:
            return "Paused";
        case TimerPrivate::Stopped:
            return "Stopped";
    }
    return "";
}

QString Timer::Note() {
    return d->note;
}

void Timer::updateFromDatabase() {
    d->timer->stop();

    QSqlQuery query;
    query.prepare("SELECT timeout, length, pausedRemaining, note FROM timers WHERE id=:id");
    query.bindValue(":id", d->id);
    query.exec();

    if (!query.next()) {
        this->deleteLater();
        return;
    }

    bool haveTimeout = !query.isNull("timeout");
    bool isPaused = !query.isNull("pausedRemaining");
    if (isPaused) {
        d->state = TimerPrivate::Paused;
        d->pauseLengthRemain = query.value("pausedRemaining").toLongLong();
    } else if (haveTimeout) {
        d->state = TimerPrivate::Running;
        d->timeoutDate = query.value("timeout").toLongLong();

        int interval = QDateTime::currentDateTimeUtc().msecsTo(QDateTime::fromMSecsSinceEpoch(d->timeoutDate));
        if (interval < 0) {
            QTimer::singleShot(0, [ = ] {
                this->doTimeout();
            });
        } else {
            d->timer->setInterval(interval);
            d->timer->start();
        }
    } else {
        d->state = TimerPrivate::Stopped;
    }
    d->note = query.value("note").toString();
    d->length = query.value("length").toLongLong();
}

void Timer::writeToDatabase() {
    QSqlQuery query;

    switch (d->state) {
        case TimerPrivate::Running: //Running
            query.prepare("UPDATE timers SET timeout=:timeout, length=:length, pausedRemaining=null, note=:note WHERE id=:id");
            query.bindValue(":timeout", d->timeoutDate);
            break;
        case TimerPrivate::Paused: //Paused
            query.prepare("UPDATE timers SET timeout=null, length=:length, pausedRemaining=:pausedRemaining, note=:note WHERE id=:id");
            query.bindValue(":pausedRemaining", d->pauseLengthRemain);
            break;
        case TimerPrivate::Stopped: //Stopped
            query.prepare("UPDATE timers SET timeout=null, length=:length, pausedRemaining=null, note=:note WHERE id=:id");
    }

    query.bindValue(":length", d->length);
    query.bindValue(":id", d->id);

    if (d->note.isEmpty()) {
        query.bindValue(":note", QVariant(QVariant::String));
    } else {
        query.bindValue(":note", d->note);
    }
    query.exec();
}

void Timer::doTimeout() {
    d->state = TimerPrivate::Stopped;
    d->timeoutDate = 0;
    d->pauseLengthRemain = 0;

    emit Timeout();
    emit StateChanged();

    tNotification* notification = new tNotification();
    notification->setAppName(tr("the24"));
    notification->setSummary(tr("Time's up!"));
    if (d->note.isEmpty()) {
        notification->setText(tr("Your timer has elapsed!"));
    } else {
        notification->setText(d->note);
    }
    notification->setCategory("x-thesuite.the24.timer-elapsed");
    notification->setTimeout(0);
    notification->post(true);

    //TODO: Play some music?

    writeToDatabase();
}
