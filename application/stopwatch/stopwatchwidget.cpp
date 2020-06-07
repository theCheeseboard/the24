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
#include "stopwatchwidget.h"
#include "ui_stopwatchwidget.h"

#include <QDBusInterface>
#include <the-libs_global.h>
#include <QElapsedTimer>
#include <QTimer>

#include "clockhelpers.h"

struct StopwatchWidgetPrivate {
    QDBusInterface* interface;

    bool paused = false;
    qulonglong initialElapsed;
    QElapsedTimer elapsed;

    QTimer* timer;
};

StopwatchWidget::StopwatchWidget(QString objectPath, QWidget* parent) :
    QWidget(parent),
    ui(new Ui::StopwatchWidget) {
    ui->setupUi(this);

    d = new StopwatchWidgetPrivate();

    ui->actionButton->setIconSize(SC_DPI_T(QSize(32, 32), QSize));

    d->interface = new QDBusInterface("com.vicr123.the24", objectPath, "com.vicr123.the24.Stopwatch", QDBusConnection::sessionBus(), this);
    QDBusConnection::sessionBus().connect("com.vicr123.the24", objectPath, "com.vicr123.the24.Stopwatch", "StateChanged", this, SLOT(update()));
    QDBusConnection::sessionBus().connect("com.vicr123.the24", objectPath, "com.vicr123.the24.Stopwatch", "Removing", this, SLOT(deleteLater()));

    d->timer = new QTimer(this);
    d->timer->setInterval(10);
    connect(d->timer, &QTimer::timeout, this, &StopwatchWidget::updateDisplay);

    update();
}

StopwatchWidget::~StopwatchWidget() {
    delete d;
    delete ui;
}

void StopwatchWidget::update() {
    QDBusPendingCallWatcher* watcher = new QDBusPendingCallWatcher(d->interface->asyncCall("Paused"));
    connect(watcher, &QDBusPendingCallWatcher::finished, this, [ = ] {
        d->paused = watcher->reply().arguments().first().toBool();
        watcher->deleteLater();

        QDBusPendingCallWatcher* remainWatcher = new QDBusPendingCallWatcher(d->interface->asyncCall("Elapsed"));
        connect(remainWatcher, &QDBusPendingCallWatcher::finished, this, [ = ] {
            ui->remainingLabel->setEnabled(false);
            d->initialElapsed = remainWatcher->reply().arguments().first().toLongLong();

            if (d->paused) {
                ui->actionButton->setIcon(QIcon::fromTheme("media-playback-start"));
                ui->remainingLabel->setEnabled(false);
                ui->resetButton->setEnabled(true);

                d->timer->stop();
            } else {
                ui->actionButton->setIcon(QIcon::fromTheme("media-playback-pause"));
                ui->remainingLabel->setEnabled(true);
                ui->resetButton->setEnabled(false);

                d->elapsed.start();
                d->timer->start();
            }

            updateDisplay();
            remainWatcher->deleteLater();
        });
    });
}

void StopwatchWidget::on_removeButton_clicked() {
    d->interface->asyncCall("Remove");
}

void StopwatchWidget::on_resetButton_clicked() {
    d->interface->asyncCall("Reset");
}

void StopwatchWidget::updateDisplay() {
    qulonglong time = d->initialElapsed;
    if (!d->paused) {
        time += d->elapsed.elapsed();
    }
    ui->remainingLabel->setText(ClockHelpers::msecsToStringWithMsecs(time));
}

void StopwatchWidget::on_actionButton_clicked() {
    if (d->paused) {
        d->interface->asyncCall("Start");
    } else {
        d->interface->asyncCall("Stop");
    }
}
