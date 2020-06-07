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
#include "ui_stopwatch.h"

#include <QDBusConnection>
#include <QDBusConnectionInterface>
#include <QDBusServiceWatcher>
#include <QDBusMessage>
#include <QDBusPendingCallWatcher>
#include "stopwatchwidget.h"

struct StopwatchPrivate {
    QList<StopwatchWidget*> stopwatches;
    QDBusServiceWatcher* watcher;
};

Stopwatch::Stopwatch(QWidget* parent) :
    QWidget(parent),
    ui(new Ui::Stopwatch) {
    ui->setupUi(this);

    d = new StopwatchPrivate();

    ui->stackedWidget->setCurrentAnimation(tStackedWidget::Fade);
    ui->stopwatchesContainer->setFixedWidth(SC_DPI(600));

    d->watcher = new QDBusServiceWatcher("com.vicr123.the24", QDBusConnection::sessionBus());
    connect(d->watcher, &QDBusServiceWatcher::serviceRegistered, this, &Stopwatch::serviceAvailable);
    connect(d->watcher, &QDBusServiceWatcher::serviceUnregistered, this, &Stopwatch::serviceUnavailable);

    if (QDBusConnection::sessionBus().interface()->isServiceRegistered("com.vicr123.the24").value()) {
        serviceAvailable();
    } else {
        serviceUnavailable();
    }

    QDBusConnection::sessionBus().connect("com.vicr123.the24", "/com/vicr123/the24", "com.vicr123.the24", "StopwatchAdded", this, SLOT(addStopwatch(QString)));
}

Stopwatch::~Stopwatch() {
    delete d;
    delete ui;
}

void Stopwatch::addStopwatch(QString path) {
    StopwatchWidget* timer = new StopwatchWidget(path);
    connect(timer, &StopwatchWidget::deleteLater, this, [ = ] {
        ui->stopwatchesLayout->removeWidget(timer);
        d->stopwatches.removeAll(timer);
    });
    ui->stopwatchesLayout->addWidget(timer);
    d->stopwatches.append(timer);
}

void Stopwatch::serviceAvailable() {
    QDBusMessage enumerate = QDBusMessage::createMethodCall("com.vicr123.the24", "/com/vicr123/the24", "com.vicr123.the24", "EnumerateStopwatches");
    QDBusPendingCallWatcher* enumerateCall = new QDBusPendingCallWatcher(QDBusConnection::sessionBus().asyncCall(enumerate));
    connect(enumerateCall, &QDBusPendingCallWatcher::finished, this, [ = ] {
        QStringList paths = enumerateCall->reply().arguments().first().toStringList();
        for (QString path : paths) {
            addStopwatch(path);
        }
        enumerateCall->deleteLater();
    });
    ui->stackedWidget->setCurrentWidget(ui->mainPage);
}

void Stopwatch::serviceUnavailable() {
    for (StopwatchWidget* stopwatch : d->stopwatches) {
        stopwatch->deleteLater();
    }
    ui->stackedWidget->setCurrentWidget(ui->serviceUnavailablePage);
}

void Stopwatch::on_addButton_clicked() {
    QDBusMessage message = QDBusMessage::createMethodCall("com.vicr123.the24", "/com/vicr123/the24", "com.vicr123.the24", "AddStopwatch");
    QDBusConnection::sessionBus().asyncCall(message);
}
