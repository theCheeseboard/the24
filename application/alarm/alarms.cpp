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
#include "alarms.h"
#include "ui_alarms.h"

#include <QDBusConnection>
#include <QDBusConnectionInterface>
#include <QDBusServiceWatcher>
#include <QDBusMessage>
#include <QDBusPendingCallWatcher>
#include <tpopover.h>
#include "alarmwidget.h"
#include "alarmpopover.h"

struct AlarmsPrivate {
    QList<AlarmWidget*> alarms;
    QDBusServiceWatcher* watcher;
};

Alarms::Alarms(QWidget* parent) :
    QWidget(parent),
    ui(new Ui::Alarms) {
    ui->setupUi(this);

    d = new AlarmsPrivate();

    ui->stackedWidget->setCurrentAnimation(tStackedWidget::Fade);
    ui->alarmsContainer->setFixedWidth(SC_DPI(600));

    d->watcher = new QDBusServiceWatcher("com.vicr123.the24", QDBusConnection::sessionBus());
    connect(d->watcher, &QDBusServiceWatcher::serviceRegistered, this, &Alarms::serviceAvailable);
    connect(d->watcher, &QDBusServiceWatcher::serviceUnregistered, this, &Alarms::serviceUnavailable);

    if (QDBusConnection::sessionBus().interface()->isServiceRegistered("com.vicr123.the24").value()) {
        serviceAvailable();
    } else {
        serviceUnavailable();
    }

    QDBusConnection::sessionBus().connect("com.vicr123.the24", "/com/vicr123/the24", "com.vicr123.the24", "AlarmAdded", this, SLOT(addAlarm(QString)));
}

Alarms::~Alarms() {
    delete d;
    delete ui;
}

void Alarms::addAlarm(QString path) {
    AlarmWidget* timer = new AlarmWidget(path);
    connect(timer, &AlarmWidget::deleteLater, this, [ = ] {
        ui->alarmsLayout->removeWidget(timer);
        d->alarms.removeAll(timer);
    });
    ui->alarmsLayout->addWidget(timer);
    d->alarms.append(timer);
}

void Alarms::serviceAvailable() {
    QDBusMessage enumerate = QDBusMessage::createMethodCall("com.vicr123.the24", "/com/vicr123/the24", "com.vicr123.the24", "EnumerateAlarms");
    QDBusPendingCallWatcher* enumerateCall = new QDBusPendingCallWatcher(QDBusConnection::sessionBus().asyncCall(enumerate));
    connect(enumerateCall, &QDBusPendingCallWatcher::finished, this, [ = ] {
        QStringList paths = enumerateCall->reply().arguments().first().toStringList();
        for (QString path : paths) {
            addAlarm(path);
        }
        enumerateCall->deleteLater();
    });
    ui->stackedWidget->setCurrentWidget(ui->mainPage);
}

void Alarms::serviceUnavailable() {
    for (AlarmWidget* alarm : d->alarms) {
        alarm->deleteLater();
    }
    d->alarms.clear();
    ui->stackedWidget->setCurrentWidget(ui->serviceUnavailablePage);
}

void Alarms::on_addButton_clicked() {
    AlarmPopover* add = new AlarmPopover("", this);
    tPopover* popover = new tPopover(add);
    popover->setPopoverWidth(SC_DPI(400));
    connect(add, &AlarmPopover::done, popover, &tPopover::dismiss);
    connect(popover, &tPopover::dismissed, popover, &tPopover::deleteLater);
    connect(popover, &tPopover::dismissed, add, &AlarmPopover::deleteLater);
    popover->show(this);
}
