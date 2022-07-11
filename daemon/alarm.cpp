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
#include "alarm.h"

#include <QDBusConnection>
#include <QSoundEffect>
#include <QSqlQuery>
#include <tnotification.h>

#include "alarmadaptor.h"

struct AlarmPrivate {
        int id;

        enum Day {
            None = 0,
            Monday = 0x1,
            Tuesday = 0x2,
            Wednesday = 0x4,
            Thursday = 0x8,
            Friday = 0x10,
            Saturday = 0x20,
            Sunday = 0x40,

            AllDays = Monday | Tuesday | Wednesday | Thursday | Friday | Saturday | Sunday

            // Keep this in sync with AlarmWidget
        };
        typedef QFlags<Day> Days;

        QTime offset;
        QTime snooze;
        Days repeatDays = None;
        bool active;
        bool ignoreNext = false;

        QSoundEffect* effect = nullptr;
};

Alarm::Alarm(int id, QObject* parent) :
    QObject(parent) {
    d = new AlarmPrivate();
    d->id = id;

    new AlarmAdaptor(this);
    QDBusConnection::sessionBus().registerObject(this->objectPath(), this);

    updateFromDatabase();
}

Alarm::~Alarm() {
    emit Removing();

    delete d;
}

QString Alarm::objectPath() {
    return QStringLiteral("/com/vicr123/the24/Alarms/%1").arg(d->id);
}

void Alarm::tick() {
    // Check if alarm is active
    if (!d->active) return;

    // Check if we should run on this day
    if (d->repeatDays != AlarmPrivate::None) {
        AlarmPrivate::Day day = static_cast<AlarmPrivate::Day>(1 << (QDate::currentDate().dayOfWeek() - 1));
        if ((d->repeatDays & day) == 0) return;
    }

    auto compareTime = [=](QTime time) {
        QTime t = QTime::currentTime();
        return t.hour() == time.hour() && t.minute() == time.minute();
    };

    if (compareTime(d->offset) || (d->snooze.isValid() && compareTime(d->snooze))) {
        d->snooze = QTime();
        emit SnoozeOffsetChanged(-1);

        if (d->ignoreNext) {
            // Ignore this one
            d->ignoreNext = false;
            emit IgnoringNextChanged(false);
        }

        prepareTone();

        // Activate!
        tNotification* notification = new tNotification();
        notification->setAppName(tr("the24"));
        notification->setSummary(tr("Alarm!"));
        notification->setText(d->offset.toString("hh:mm"));
        notification->setCategory("x-thesuite.the24.alarm-elapsed");
        notification->insertAction("snooze", tr("Snooze"));
        notification->setTimeout(0);
        connect(notification, &tNotification::actionClicked, this, [=](QString key) {
            if (key == "snooze") {
                // Snooze the alarm
                d->snooze = QTime::currentTime().addSecs(600); // 10 minutes
                emit SnoozeOffsetChanged(d->snooze.msecsSinceStartOfDay());

                if (d->effect) {
                    d->effect->stop();
                    d->effect->deleteLater();
                    d->effect = nullptr;
                }
            }
        });
        connect(notification, &tNotification::dismissed, this, [=] {
            if (d->repeatDays == AlarmPrivate::None && !d->snooze.isValid()) this->SetActive(false);

            if (d->effect) {
                d->effect->stop();
                d->effect->deleteLater();
                d->effect = nullptr;
            }
        });
        notification->post(true);

        d->effect->play();
    }
}

void Alarm::Remove() {
    QSqlQuery query;
    query.prepare("DELETE FROM alarms WHERE id=:id");
    query.bindValue(":id", d->id);
    query.exec();

    this->deleteLater();
}

bool Alarm::Active() {
    return d->active;
}

qulonglong Alarm::Offset() {
    return d->offset.msecsSinceStartOfDay();
}

qulonglong Alarm::Repeat() {
    return d->repeatDays;
}

qlonglong Alarm::SnoozeOffset() {
    if (d->snooze.isValid()) {
        return d->snooze.msecsSinceStartOfDay();
    } else {
        return -1;
    }
}

bool Alarm::WillIgnoreNext() {
    return d->ignoreNext;
}

void Alarm::SetActive(bool active) {
    d->active = active;

    if (d->ignoreNext) {
        d->ignoreNext = false;
        emit IgnoringNextChanged(false);
    }

    if (d->snooze.isValid()) {
        d->snooze = QTime();
        emit SnoozeOffsetChanged(-1);
    }

    emit ActiveChanged(active);
    writeToDatabase();
}

void Alarm::SetOffset(qulonglong offset) {
    d->offset = QTime::fromMSecsSinceStartOfDay(offset);

    if (d->ignoreNext) {
        d->ignoreNext = false;
        emit IgnoringNextChanged(false);
    }

    if (d->snooze.isValid()) {
        d->snooze = QTime();
        emit SnoozeOffsetChanged(-1);
    }

    emit OffsetChanged(offset);
    writeToDatabase();
}

void Alarm::SetRepeat(qulonglong repeat) {
    d->repeatDays = static_cast<AlarmPrivate::Days>(static_cast<int>(repeat));

    if (d->ignoreNext) {
        d->ignoreNext = false;
        emit IgnoringNextChanged(false);
    }

    if (d->snooze.isValid()) {
        d->snooze = QTime();
        emit SnoozeOffsetChanged(-1);
    }

    emit RepeatChanged(d->repeatDays);
    writeToDatabase();
}

void Alarm::IgnoreNext() {
    if (d->snooze.isValid()) {
        d->snooze = QTime();
        emit SnoozeOffsetChanged(-1);
    } else {
        d->ignoreNext = true;
        emit IgnoringNextChanged(true);
    }
}

void Alarm::updateFromDatabase() {
    QSqlQuery query;
    query.prepare("SELECT offset, repeat, active FROM alarms WHERE id=:id");
    query.bindValue(":id", d->id);
    query.exec();

    if (!query.next()) {
        this->deleteLater();
        return;
    }

    d->offset = QTime::fromMSecsSinceStartOfDay(query.value("offset").toULongLong());
    d->repeatDays = static_cast<AlarmPrivate::Days>(query.value("repeat").toInt());
    d->active = query.value("active").toBool();
}

void Alarm::writeToDatabase() {
    QSqlQuery query;
    query.prepare("UPDATE alarms SET offset=:offset, repeat=:repeat, active=:active WHERE id=:id");
    query.bindValue(":id", d->id);
    query.bindValue(":offset", d->offset.msecsSinceStartOfDay());
    query.bindValue(":repeat", static_cast<int>(d->repeatDays));
    query.bindValue(":active", d->active);
    query.exec();
}

void Alarm::prepareTone() {
    if (!d->effect) {
        d->effect = new QSoundEffect();
        d->effect->setSource(QUrl::fromLocalFile("/usr/share/sounds/the24/tones/silly.wav"));
        d->effect->setLoopCount(QSoundEffect::Infinite);
    }
}
