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
#include "ui_timer.h"

#include <QDBusConnection>
#include <QDBusConnectionInterface>
#include <QDBusMessage>
#include <QDBusServiceWatcher>

#include <tpopover.h>
#include "newtimerpopover.h"

#include "timerwidget.h"

struct TimerPrivate {
    QList<TimerWidget*> timers;
    QDBusServiceWatcher* watcher;
};

Timer::Timer(QWidget* parent) :
    QWidget(parent),
    ui(new Ui::Timer) {
    ui->setupUi(this);

    d = new TimerPrivate();

    ui->stackedWidget->setCurrentAnimation(tStackedWidget::Fade);
    ui->timersContainer->setFixedWidth(SC_DPI(600));

    d->watcher = new QDBusServiceWatcher("com.vicr123.the24", QDBusConnection::sessionBus());
    connect(d->watcher, &QDBusServiceWatcher::serviceRegistered, this, &Timer::serviceAvailable);
    connect(d->watcher, &QDBusServiceWatcher::serviceUnregistered, this, &Timer::serviceUnavailable);

    if (QDBusConnection::sessionBus().interface()->isServiceRegistered("com.vicr123.the24").value()) {
        serviceAvailable();
    } else {
        serviceUnavailable();
    }

    QDBusConnection::sessionBus().connect("com.vicr123.the24", "/com/vicr123/the24", "com.vicr123.the24", "TimerAdded", this, SLOT(addTimer(QString)));
}

Timer::~Timer() {
    delete d;
    delete ui;
}

void Timer::addTimer(QString path) {
    TimerWidget* timer = new TimerWidget(path);
    connect(timer, &TimerWidget::deleteLater, this, [ = ] {
        ui->timersLayout->removeWidget(timer);
        d->timers.removeAll(timer);
    });
    ui->timersLayout->addWidget(timer);
    d->timers.append(timer);
}

void Timer::serviceAvailable() {
    QDBusMessage enumerate = QDBusMessage::createMethodCall("com.vicr123.the24", "/com/vicr123/the24", "com.vicr123.the24", "EnumerateTimers");
    QDBusPendingCallWatcher* enumerateCall = new QDBusPendingCallWatcher(QDBusConnection::sessionBus().asyncCall(enumerate));
    connect(enumerateCall, &QDBusPendingCallWatcher::finished, this, [ = ] {
        QStringList paths = enumerateCall->reply().arguments().first().toStringList();
        for (QString path : paths) {
            addTimer(path);
        }
        enumerateCall->deleteLater();
    });
    ui->stackedWidget->setCurrentWidget(ui->mainPage);
}

void Timer::serviceUnavailable() {
    for (TimerWidget* timer : d->timers) {
        timer->deleteLater();
    }
    ui->stackedWidget->setCurrentWidget(ui->serviceUnavailablePage);
}

void Timer::on_addButton_clicked() {
    NewTimerPopover* add = new NewTimerPopover(this);
    tPopover* popover = new tPopover(add);
    popover->setPopoverWidth(SC_DPI(400));
    connect(add, &NewTimerPopover::done, popover, &tPopover::dismiss);
    connect(popover, &tPopover::dismissed, popover, &tPopover::deleteLater);
    connect(popover, &tPopover::dismissed, add, &NewTimerPopover::deleteLater);
    popover->show(this);
}
