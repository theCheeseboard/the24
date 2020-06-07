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
#include "stopwatch.h"

#include <QSqlQuery>
#include <QVariant>
#include <QDBusConnection>
#include <QElapsedTimer>

#include <tnotification.h>

#include "stopwatch_adaptor.h"

struct StopwatchPrivate {
    int id;

    QDateTime startTime;
    QElapsedTimer elapsed;
    qulonglong offset = 0;
    bool paused = false;
};

Stopwatch::Stopwatch(int id, QObject* parent) : QObject(parent) {
    d = new StopwatchPrivate();
    d->id = id;

    updateFromDatabase();
    updateRunningValues();
    writeToDatabase();

    new StopwatchAdaptor(this);
    QDBusConnection::sessionBus().registerObject(this->objectPath(), this);
}

Stopwatch::~Stopwatch() {
    emit Removing();

    delete d;
}

QString Stopwatch::objectPath() {
    return QStringLiteral("/com/vicr123/the24/Stopwatches/%1").arg(d->id);
}

void Stopwatch::Remove() {
    QSqlQuery query;
    query.prepare("DELETE FROM stopwatch WHERE id=:id");
    query.bindValue(":id", d->id);
    query.exec();

    this->deleteLater();
}

qulonglong Stopwatch::Elapsed() {
    if (d->paused) {
        return d->offset;
    } else {
        return d->elapsed.elapsed() + d->offset;
    }
}

bool Stopwatch::Paused() {
    return d->paused;
}

void Stopwatch::Start() {
    if (!d->paused) {
        sendErrorReply(QDBusError::NotSupported, "Can't start a timer that's running");
        return;
    }

    d->startTime = QDateTime::currentDateTimeUtc();
    d->paused = false;
    d->elapsed.start();

    emit StateChanged();
    writeToDatabase();
}

void Stopwatch::Stop() {
    if (d->paused) {
        sendErrorReply(QDBusError::NotSupported, "Can't stop a timer that's not running");
        return;
    }

    updateRunningValues();
    d->paused = true;

    emit StateChanged();
    writeToDatabase();
}

void Stopwatch::Reset() {
    d->offset = 0;
    writeToDatabase();

    QSqlQuery query;
    query.prepare("DELETE FROM laps WHERE stopwatch=:id");
    query.bindValue(":id", d->id);
    query.exec();

    emit StateChanged();
}

void Stopwatch::Lap() {
    if (d->paused) {
        sendErrorReply(QDBusError::NotSupported, "Can't lap while the stopwatch is paused");
        return;
    }

    qulonglong elapsed = this->Elapsed();

    QSqlQuery query;
    query.prepare("INSERT INTO laps(stopwatch, offset) VALUES(:id, :offset)");
    query.bindValue(":id", d->id);
    query.bindValue(":offset", elapsed);
    query.exec();

    updateRunningValues();
    emit LapsChanged();

    writeToDatabase();
}

QList<qulonglong> Stopwatch::Laps() {
    QSqlQuery query;
    query.prepare("SELECT offset FROM laps WHERE stopwatch=:id");
    query.bindValue(":id", d->id);
    query.exec();

    QList<qulonglong> offsets;
    while (query.next()) {
        offsets.append(query.value("offset").toULongLong());
    }

    return offsets;
}

void Stopwatch::updateFromDatabase() {
    QSqlQuery query;
    query.prepare("SELECT starttime, offset, paused FROM stopwatch WHERE id=:id");
    query.bindValue(":id", d->id);
    query.exec();

    if (!query.next()) {
        this->deleteLater();
        return;
    }

    d->startTime = QDateTime::fromMSecsSinceEpoch(query.value("starttime").toULongLong());
    d->offset = query.value("offset").toULongLong();
    d->paused = query.value("paused").toBool();
}

void Stopwatch::updateRunningValues() {
    QDateTime current = QDateTime::currentDateTimeUtc();

    if (d->startTime > current) {
        //Bail out here
        d->paused = true;
        d->offset = 0;

        writeToDatabase();

        tNotification* notification = new tNotification();
        notification->setAppName(tr("the24"));
        notification->setSummary(tr("Stopwatch"));
        notification->setText(tr("A stopwatch was reset because the system clock was changed unexpectedly."));
        notification->post(true);

        return;
    }

    qulonglong difference;
    if (d->elapsed.isValid()) {
        difference = d->elapsed.restart();
    } else {
        difference = d->startTime.msecsTo(current);
        d->elapsed.start();
    }
    d->offset += difference;
    d->startTime = current;
}

void Stopwatch::writeToDatabase() {
    QSqlQuery query;
    query.prepare("UPDATE stopwatch SET starttime=:starttime, offset=:offset, paused=:paused WHERE id=:id");
    query.bindValue(":id", d->id);
    query.bindValue(":starttime", d->startTime.toMSecsSinceEpoch());
    query.bindValue(":offset", d->offset);
    query.bindValue(":paused", d->paused);
    query.exec();
}
