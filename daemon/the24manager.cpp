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
#include "the24_adaptor.h"

struct The24ManagerPrivate {
    tSettings settings;
    QList<Timer*> timers;
};

The24Manager::The24Manager(QObject* parent) : QObject(parent) {
    d = new The24ManagerPrivate();

    new The24Adaptor(this);
    QDBusConnection::sessionBus().registerObject("/com/vicr123/the24", this);

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
}

The24Manager::~The24Manager() {
    delete d;
}

QStringList The24Manager::EnumerateAlarms() {
    return {};
}

QStringList The24Manager::EnumerateTimers() {
    QStringList objectPaths;
    for (Timer* timer : d->timers) {
        objectPaths.append(timer->objectPath());
    }
    return objectPaths;
}

QStringList The24Manager::EnumerateStopwatches() {
    return {};
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

void The24Manager::RequestExit() {
    QCoreApplication::exit();
}
