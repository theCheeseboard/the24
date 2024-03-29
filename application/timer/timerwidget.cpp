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
#include "timerwidget.h"
#include "ui_timerwidget.h"

#include <QCoroDBusPendingCall>
#include <QDBusInterface>
#include <QTime>
#include <QTimer>
#include <libcontemporary_global.h>

#include "clockhelpers.h"

struct TimerWidgetPrivate {
        QDBusInterface* interface;

        QString state;
        qlonglong msecsLeft;
        QTimer* timer;
};

TimerWidget::TimerWidget(QString objectPath, QWidget* parent) :
    QWidget(parent),
    ui(new Ui::TimerWidget) {
    ui->setupUi(this);
    d = new TimerWidgetPrivate();

    ui->actionButton->setIconSize(SC_DPI_T(QSize(32, 32), QSize));

    d->interface = new QDBusInterface("com.vicr123.the24", objectPath, "com.vicr123.the24.Timer", QDBusConnection::sessionBus(), this);
    QDBusConnection::sessionBus().connect("com.vicr123.the24", objectPath, "com.vicr123.the24.Timer", "StateChanged", this, SLOT(update()));
    QDBusConnection::sessionBus().connect("com.vicr123.the24", objectPath, "com.vicr123.the24.Timer", "LengthChanged", this, SLOT(update()));
    QDBusConnection::sessionBus().connect("com.vicr123.the24", objectPath, "com.vicr123.the24.Timer", "NoteChanged", this, SLOT(update()));
    QDBusConnection::sessionBus().connect("com.vicr123.the24", objectPath, "com.vicr123.the24.Timer", "Removing", this, SLOT(deleteLater()));

    d->timer = new QTimer(this);
    d->timer->setInterval(1000);
    connect(d->timer, &QTimer::timeout, this, [=] {
        d->msecsLeft -= 1000;
        updateDisplay();
    });

    update();
}

TimerWidget::~TimerWidget() {
    delete d;
    delete ui;
}

QCoro::Task<> TimerWidget::update() {
    d->timer->stop();

    auto stateReply = co_await d->interface->asyncCall("State");
    d->state = stateReply.arguments().first().toString();

    auto msRemainReply = co_await d->interface->asyncCall("MillisecondsRemaining");
    d->msecsLeft = msRemainReply.arguments().first().toLongLong();

    if (d->state == "Running") {
        ui->actionButton->setIcon(QIcon::fromTheme("media-playback-pause"));
        ui->remainingLabel->setEnabled(true);
        d->timer->start();

        ui->resetButton->setEnabled(false);
    } else if (d->state == "Paused") {
        ui->actionButton->setIcon(QIcon::fromTheme("media-playback-start"));
        ui->remainingLabel->setEnabled(false);
        ui->resetButton->setEnabled(true);
    } else {
        ui->actionButton->setIcon(QIcon::fromTheme("media-playback-start"));
        ui->remainingLabel->setEnabled(true);
        ui->resetButton->setEnabled(false);
    }

    updateDisplay();
}

void TimerWidget::updateDisplay() {
    ui->remainingLabel->setText(ClockHelpers::msecsToString(d->msecsLeft));
}

void TimerWidget::on_actionButton_clicked() {
    if (d->state == "Running") {
        d->interface->asyncCall("Pause");
    } else {
        d->interface->asyncCall("Resume");
    }
}

void TimerWidget::on_resetButton_clicked() {
    d->interface->asyncCall("Reset");
}

void TimerWidget::on_removeButton_clicked() {
    d->interface->asyncCall("Remove");
}
