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
#include "worldclockwidget.h"
#include "ui_worldclockwidget.h"

#include <QTimer>
#include <QMenu>
#include <QContextMenuEvent>

struct WorldClockWidgetPrivate {
    QTimeZone tz;
    QTimer* ticker;

    bool removable = true;
};

WorldClockWidget::WorldClockWidget(QTimeZone tz, QWidget* parent) :
    QWidget(parent),
    ui(new Ui::WorldClockWidget) {
    ui->setupUi(this);
    d = new WorldClockWidgetPrivate();
    d->tz = tz;

    d->ticker = new QTimer(this);
    d->ticker->setInterval(1000);
    connect(d->ticker, &QTimer::timeout, this, &WorldClockWidget::updateClock);
    d->ticker->start();
    updateClock();
}

WorldClockWidget::~WorldClockWidget() {
    delete d;
    delete ui;
}

void WorldClockWidget::setRemovable(bool removable) {
    d->removable = removable;
}

void WorldClockWidget::contextMenuEvent(QContextMenuEvent* event) {
    if (!d->removable) return;

    QMenu* menu = new QMenu();
    menu->addSection(tr("For this clock"));
    menu->addAction(QIcon::fromTheme("list-remove"), tr("Remove"), this, &WorldClockWidget::remove);

    connect(menu, &QMenu::aboutToHide, menu, &QMenu::deleteLater);
    menu->popup(event->globalPos());
}

void WorldClockWidget::updateClock() {
    QDateTime date = QDateTime::currentDateTimeUtc();
    date = date.addSecs(d->tz.offsetFromUtc(date));
    ui->timeLabel->setText(QLocale().toString(date, "hh:mm"));

    int dayDifference = QDateTime::currentDateTime().daysTo(date);
    if (dayDifference == 0) {
        ui->dayDifferenceLabel->setVisible(false);
    } else if (dayDifference < 0) {
        ui->dayDifferenceLabel->setText(QStringLiteral("-%1").arg(dayDifference * -1));
        ui->dayDifferenceLabel->setVisible(true);
    } else {
        ui->dayDifferenceLabel->setText(QStringLiteral("+%1").arg(dayDifference));
        ui->dayDifferenceLabel->setVisible(true);
    }

    ui->timezoneLabel->setText(d->tz.displayName(QTimeZone::GenericTime, QTimeZone::LongName));
}
