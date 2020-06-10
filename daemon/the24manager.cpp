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
#include "the24manager.h"

#include <tsettings.h>
#include <QDBusConnection>
#include <QJsonDocument>
#include <QJsonObject>

#include <QSqlQuery>
#include <QSqlError>

#include "timer.h"
#include "stopwatch.h"
#include "alarm.h"
#include "the24_adaptor.h"

struct The24ManagerPrivate {
    tSettings settings;
    QList<Timer*> timers;
    QList<Stopwatch*> stopwatches;
    QList<Alarm*> alarms;

    QTimer* alarmTimer;
};

The24Manager::The24Manager(QObject* parent) : QObject(parent) {
    d = new The24ManagerPrivate();

    new The24Adaptor(this);
    QDBusConnection::sessionBus().registerObject("/com/vicr123/the24", this);

    d->alarmTimer = new QTimer(this);
    d->alarmTimer->setInterval((60 - QTime::currentTime().second()) * 1000);
    connect(d->alarmTimer, &QTimer::timeout, this, [ = ] {
        d->alarmTimer->setInterval(60000);
        for (Alarm* alarm : d->alarms) {
            alarm->tick();
        }
    });
    d->alarmTimer->start();

    //Initialise timers
    QSqlQuery timerQuery("SELECT id FROM timers");
    while (timerQuery.next()) {
        Timer* timer = new Timer(timerQuery.value("id").toInt());
        QString objectPath = timer->objectPath();
        connect(timer, &Timer::destroyed, this, [ = ] {
            emit TimerRemoved(objectPath);
            d->timers.removeAll(timer);
        });
        d->timers.append(timer);
    }

    //Initialise stopwatches
    QSqlQuery stopwatchQuery("SELECT id FROM stopwatch");
    while (stopwatchQuery.next()) {
        Stopwatch* stopwatch = new Stopwatch(stopwatchQuery.value("id").toInt());
        QString objectPath = stopwatch->objectPath();
        connect(stopwatch, &Stopwatch::destroyed, this, [ = ] {
            emit StopwatchRemoved(objectPath);
            d->stopwatches.removeAll(stopwatch);
        });
        d->stopwatches.append(stopwatch);
    }

    //Initialise stopwatches
    QSqlQuery alarmQuery("SELECT id FROM alarms");
    while (alarmQuery.next()) {
        Alarm* alarm = new Alarm(alarmQuery.value("id").toInt());
        QString objectPath = alarm->objectPath();
        connect(alarm, &Alarm::destroyed, this, [ = ] {
            emit AlarmRemoved(objectPath);
            d->alarms.removeAll(alarm);
        });
        d->alarms.append(alarm);
    }
}

The24Manager::~The24Manager() {
    delete d;
}

QStringList The24Manager::EnumerateAlarms() {
    QStringList objectPaths;
    for (Alarm* alarm : d->alarms) {
        objectPaths.append(alarm->objectPath());
    }
    return objectPaths;
}

QStringList The24Manager::EnumerateTimers() {
    QStringList objectPaths;
    for (Timer* timer : d->timers) {
        objectPaths.append(timer->objectPath());
    }
    return objectPaths;
}

QStringList The24Manager::EnumerateStopwatches() {
    QStringList objectPaths;
    for (Stopwatch* stopwatch : d->stopwatches) {
        objectPaths.append(stopwatch->objectPath());
    }
    return objectPaths;
}

QString The24Manager::AddTimer(qint64 msecsFromNow) {
    qint64 msecs = QDateTime::currentMSecsSinceEpoch() + msecsFromNow;
    QSqlQuery query;
    query.prepare("INSERT INTO timers(timeout, length) VALUES(:timeout, :length)");
    query.bindValue(":timeout", msecs);
    query.bindValue(":length", msecsFromNow);
    query.exec();

    Timer* timer = new Timer(query.lastInsertId().toInt());
    QString objectPath = timer->objectPath();
    connect(timer, &Timer::destroyed, this, [ = ] {
        emit TimerRemoved(objectPath);
        d->timers.removeAll(timer);
    });
    d->timers.append(timer);
    emit TimerAdded(objectPath);

    return timer->objectPath();
}

QString The24Manager::AddStopwatch() {
    QSqlQuery query;
    query.prepare("INSERT INTO stopwatch(starttime) VALUES(:starttime)");
    query.bindValue(":starttime", QDateTime::currentMSecsSinceEpoch());
    query.exec();

    Stopwatch* stopwatch = new Stopwatch(query.lastInsertId().toInt());
    QString objectPath = stopwatch->objectPath();
    connect(stopwatch, &Stopwatch::destroyed, this, [ = ] {
        emit StopwatchRemoved(objectPath);
        d->stopwatches.removeAll(stopwatch);
    });
    d->stopwatches.append(stopwatch);
    emit StopwatchAdded(objectPath);

    return stopwatch->objectPath();
}

QString The24Manager::AddAlarm(quint64 offset, quint64 repeats) {
    QSqlQuery query;
    query.prepare("INSERT INTO alarms(offset, repeat) VALUES(:offset, :repeat)");
    query.bindValue(":offset", offset);
    query.bindValue(":repeat", repeats);
    query.exec();

    Alarm* alarm = new Alarm(query.lastInsertId().toInt());
    QString objectPath = alarm->objectPath();
    connect(alarm, &Alarm::destroyed, this, [ = ] {
        emit AlarmRemoved(objectPath);
        d->alarms.removeAll(alarm);
    });
    d->alarms.append(alarm);
    emit AlarmAdded(objectPath);

    return alarm->objectPath();
}

void The24Manager::RequestExit() {
    QCoreApplication::exit();
}
