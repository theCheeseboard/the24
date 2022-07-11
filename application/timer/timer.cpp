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

#include <QCoroDBusPendingCall>
#include <QDBusConnection>
#include <QDBusConnectionInterface>
#include <QDBusMessage>
#include <QDBusServiceWatcher>
#include <QTimer>

#include "newtimerpopover.h"
#include <tpopover.h>

#include "timerwidget.h"

using std::chrono_literals::operator""min;
using std::chrono_literals::operator""h;

struct TimerPrivate {
        QList<TimerWidget*> timers;
        QDBusServiceWatcher* watcher;
        bool available;
};

Timer::Timer(QWidget* parent) :
    QWidget(parent),
    ui(new Ui::Timer) {
    ui->setupUi(this);

    d = new TimerPrivate();

    ui->stackedWidget->setCurrentAnimation(tStackedWidget::Fade);
    ui->timersContainer->setFixedWidth(SC_DPI(600));
    ui->splashImage->setPixmap(QIcon::fromTheme("clock-timer").pixmap(SC_DPI_T(QSize(128, 128), QSize)));

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
    for (auto widget : d->timers) widget->disconnect(this);
    delete d;
    delete ui;
}

void Timer::addTimer(QString path) {
    TimerWidget* timer = new TimerWidget(path);
    connect(timer, &TimerWidget::destroyed, this, [this, timer] {
        ui->timersLayout->removeWidget(timer);
        d->timers.removeAll(timer);
        if (d->timers.count() == 0 && d->available) {
            ui->stackedWidget->setCurrentWidget(ui->emptyPage);
        }
    });
    ui->timersLayout->addWidget(timer);
    d->timers.append(timer);
    ui->stackedWidget->setCurrentWidget(ui->mainPage);
}

QCoro::Task<> Timer::serviceAvailable() {
    d->available = true;
    QDBusMessage enumerate = QDBusMessage::createMethodCall("com.vicr123.the24", "/com/vicr123/the24", "com.vicr123.the24", "EnumerateTimers");
    auto reply = co_await QDBusConnection::sessionBus().asyncCall(enumerate);

    QStringList paths = reply.arguments().first().toStringList();
    for (QString path : paths) {
        addTimer(path);
    }
}

void Timer::serviceUnavailable() {
    d->available = false;
    for (TimerWidget* timer : d->timers) {
        timer->deleteLater();
    }
    d->timers.clear();
    ui->stackedWidget->setCurrentWidget(ui->serviceUnavailablePage);
}

void Timer::startTimer(std::chrono::milliseconds msecs) {
    QDBusMessage message = QDBusMessage::createMethodCall("com.vicr123.the24", "/com/vicr123/the24", "com.vicr123.the24", "AddTimer");
    message.setArguments({static_cast<qint64>(msecs.count())});
    QDBusConnection::sessionBus().asyncCall(message);
}

void Timer::on_addButton_clicked() {
    NewTimerPopover* add = new NewTimerPopover(this);
    tPopover* popover = new tPopover(add);
    popover->setPopoverWidth(SC_DPI_W(400, this));
    connect(add, &NewTimerPopover::done, popover, &tPopover::dismiss);
    connect(popover, &tPopover::dismissed, popover, &tPopover::deleteLater);
    connect(popover, &tPopover::dismissed, add, &NewTimerPopover::deleteLater);
    popover->show(this->window());
}

void Timer::on_oneMinButton_clicked() {
    startTimer(1min);
}

void Timer::on_twoMinButton_clicked() {
    startTimer(2min);
}

void Timer::on_fiveMinButton_clicked() {
    startTimer(5min);
}

void Timer::on_tenMinButton_clicked() {
    startTimer(10min);
}

void Timer::on_fifteenMinButton_clicked() {
    startTimer(15min);
}

void Timer::on_thirtyMinButton_clicked() {
    startTimer(30min);
}

void Timer::on_oneHrButton_clicked() {
    startTimer(1h);
}

void Timer::on_twoHrButton_clicked() {
    startTimer(2h);
}

void Timer::on_customAddButton_clicked() {
    ui->addButton->click();
}
