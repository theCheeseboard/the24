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
#include "worldclock.h"
#include "ui_worldclock.h"

#include <the-libs_global.h>
#include "worldclockwidget.h"

#include <tsettings.h>
#include <tpopover.h>
#include "addclockpopover.h"

struct WorldClockPrivate {
    QList<WorldClockWidget*> clocks;
    tSettings settings;
};

WorldClock::WorldClock(QWidget* parent) :
    QWidget(parent),
    ui(new Ui::WorldClock) {
    ui->setupUi(this);

    d = new WorldClockPrivate();

    ui->clocksContainer->setFixedWidth(SC_DPI(600));

    connect(&d->settings, &tSettings::settingChanged, this, [ = ](QString key) {
        if (key == "WorldClock/timezones") {
            updateClocks();
        }
    });
    updateClocks();
}

WorldClock::~WorldClock() {
    delete d;
    delete ui;
}

void WorldClock::on_addButton_clicked() {
    AddClockPopover* add = new AddClockPopover(this);
    tPopover* popover = new tPopover(add);
    popover->setPopoverWidth(SC_DPI(600));
    connect(add, &AddClockPopover::done, popover, &tPopover::dismiss);
    connect(add, &AddClockPopover::addClock, this, [ = ](QByteArray tz) {
        QStringList clocks = d->settings.delimitedList("WorldClock/timezones");
        clocks.append(tz);
        d->settings.setDelimitedList("WorldClock/timezones", clocks);
    });
    connect(popover, &tPopover::dismissed, popover, &tPopover::deleteLater);
    connect(popover, &tPopover::dismissed, add, &AddClockPopover::deleteLater);
    popover->show(this);
}

void WorldClock::updateClocks() {
    for (WorldClockWidget* wc : d->clocks) {
        ui->clocksLayout->removeWidget(wc);
        wc->deleteLater();
    }
    d->clocks.clear();

    WorldClockWidget* wc = new WorldClockWidget(QTimeZone::systemTimeZone());
    wc->setRemovable(false);
    ui->clocksLayout->addWidget(wc);
    d->clocks.append(wc);

    for (QString tz : d->settings.delimitedList("WorldClock/timezones")) {
        WorldClockWidget* wc = new WorldClockWidget(QTimeZone(tz.toUtf8()));
        connect(wc, &WorldClockWidget::remove, this, [ = ] {
            QStringList clocks = d->settings.delimitedList("WorldClock/timezones");
            clocks.removeAll(tz);
            d->settings.setDelimitedList("WorldClock/timezones", clocks);
        });
        ui->clocksLayout->addWidget(wc);
        d->clocks.append(wc);
    }
}
