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
#include "alarmwidget.h"
#include "ui_alarmwidget.h"

#include <QContextMenuEvent>
#include <QDBusInterface>
#include <QMenu>
#include <QTime>

#include "alarmpopover.h"
#include <tpopover.h>

struct AlarmWidgetPrivate {
        QDBusInterface* interface;

        enum Day {
            None = 0,
            Monday = 0x1,
            Tuesday = 0x2,
            Wednesday = 0x4,
            Thursday = 0x8,
            Friday = 0x10,
            Saturday = 0x20,
            Sunday = 0x40,

            AllDays = Monday | Tuesday | Wednesday | Thursday | Friday | Saturday | Sunday

            // Keep this in sync with Alarm
        };
        typedef QFlags<Day> Days;

        QTime offset;
        QString objectPath;
};

AlarmWidget::AlarmWidget(QString objectPath, QWidget* parent) :
    QWidget(parent),
    ui(new Ui::AlarmWidget) {
    ui->setupUi(this);

    d = new AlarmWidgetPrivate();
    d->objectPath = objectPath;

    d->interface = new QDBusInterface("com.vicr123.the24", objectPath, "com.vicr123.the24.Alarm", QDBusConnection::sessionBus(), this);
    QDBusConnection::sessionBus().connect("com.vicr123.the24", objectPath, "com.vicr123.the24.Alarm", "ActiveChanged", this, SLOT(update()));
    QDBusConnection::sessionBus().connect("com.vicr123.the24", objectPath, "com.vicr123.the24.Alarm", "OffsetChanged", this, SLOT(update()));
    QDBusConnection::sessionBus().connect("com.vicr123.the24", objectPath, "com.vicr123.the24.Alarm", "SnoozeOffsetChanged", this, SLOT(update()));
    QDBusConnection::sessionBus().connect("com.vicr123.the24", objectPath, "com.vicr123.the24.Alarm", "RepeatChanged", this, SLOT(update()));
    QDBusConnection::sessionBus().connect("com.vicr123.the24", objectPath, "com.vicr123.the24.Alarm", "Removing", this, SLOT(deleteLater()));

    update();
}

AlarmWidget::~AlarmWidget() {
    delete d;
    delete ui;
}

void AlarmWidget::update() {
    QDBusPendingCallWatcher* activeWatcher = new QDBusPendingCallWatcher(d->interface->asyncCall("Active"));
    connect(activeWatcher, &QDBusPendingCallWatcher::finished, this, [=] {
        bool active = activeWatcher->reply().arguments().first().toBool();
        ui->activeSwitch->setChecked(active);
        ui->timeLabel->setEnabled(active);
        activeWatcher->deleteLater();
    });

    QDBusPendingCallWatcher* offsetWatcher = new QDBusPendingCallWatcher(d->interface->asyncCall("Offset"));
    connect(offsetWatcher, &QDBusPendingCallWatcher::finished, this, [=] {
        d->offset = QTime::fromMSecsSinceStartOfDay(offsetWatcher->reply().arguments().first().toULongLong());
        ui->timeLabel->setText(d->offset.toString("hh:mm"));

        offsetWatcher->deleteLater();
    });

    QDBusPendingCallWatcher* repeatsWatcher = new QDBusPendingCallWatcher(d->interface->asyncCall("Repeat"));
    connect(repeatsWatcher, &QDBusPendingCallWatcher::finished, this, [=] {
        AlarmWidgetPrivate::Days repeats = static_cast<AlarmWidgetPrivate::Days>(repeatsWatcher->reply().arguments().first().toInt());
        QLocale l;

        AlarmWidgetPrivate::Days weekdayDays = AlarmWidgetPrivate::None;
        for (Qt::DayOfWeek day : l.weekdays()) {
            weekdayDays |= static_cast<AlarmWidgetPrivate::Day>(1 << (day - 1));
        }

        bool alldays = repeats == AlarmWidgetPrivate::AllDays;
        bool weekdays = repeats == weekdayDays;
        bool weekends = repeats == (~weekdayDays & AlarmWidgetPrivate::AllDays);

        if (repeats == AlarmWidgetPrivate::None) {
            ui->repeatsLabel->setVisible(false);
        } else if (alldays) {
            ui->repeatsLabel->setText(tr("Repeats every day"));
            ui->repeatsLabel->setVisible(true);
        } else if (weekdays) {
            ui->repeatsLabel->setText(tr("Repeats on Weekdays"));
            ui->repeatsLabel->setVisible(true);
        } else if (weekends) {
            ui->repeatsLabel->setText(tr("Repeats on Weekends"));
            ui->repeatsLabel->setVisible(true);
        } else {
            QStringList repeatsList;
            for (int i = l.firstDayOfWeek() - 1; i < l.firstDayOfWeek() + 6; i++) {
                int day = i % 7;
                if (repeats & 1 << day) repeatsList.append(l.dayName(day + 1, QLocale::ShortFormat));
            }
            //            if (repeats & AlarmWidgetPrivate::Monday) repeatsList.append(l.dayName(1, QLocale::ShortFormat));
            //            if (repeats & AlarmWidgetPrivate::Tuesday) repeatsList.append(l.dayName(2, QLocale::ShortFormat));
            //            if (repeats & AlarmWidgetPrivate::Wednesday) repeatsList.append(l.dayName(3, QLocale::ShortFormat));
            //            if (repeats & AlarmWidgetPrivate::Thursday) repeatsList.append(l.dayName(4, QLocale::ShortFormat));
            //            if (repeats & AlarmWidgetPrivate::Friday) repeatsList.append(l.dayName(5, QLocale::ShortFormat));
            //            if (repeats & AlarmWidgetPrivate::Saturday) repeatsList.append(l.dayName(6, QLocale::ShortFormat));
            //            if (repeats & AlarmWidgetPrivate::Sunday) repeatsList.append(l.dayName(7, QLocale::ShortFormat));
            ui->repeatsLabel->setText(tr("Repeats on %1").arg(l.createSeparatedList(repeatsList)));
            ui->repeatsLabel->setVisible(true);
        }

        repeatsWatcher->deleteLater();
    });

    QDBusPendingCallWatcher* snoozeOffsetWatcher = new QDBusPendingCallWatcher(d->interface->asyncCall("SnoozeOffset"));
    connect(snoozeOffsetWatcher, &QDBusPendingCallWatcher::finished, this, [=] {
        qlonglong offset = snoozeOffsetWatcher->reply().arguments().first().toLongLong();
        if (offset < 0) {
            ui->snoozeLabel->setVisible(false);
        } else {
            QTime snoozeOffset = QTime::fromMSecsSinceStartOfDay(snoozeOffsetWatcher->reply().arguments().first().toLongLong());
            ui->snoozeLabel->setText(tr("Snoozed until %1").arg(snoozeOffset.toString("hh:mm")));
            ui->snoozeLabel->setVisible(true);
        }

        snoozeOffsetWatcher->deleteLater();
    });
}

void AlarmWidget::on_activeSwitch_toggled(bool checked) {
    d->interface->asyncCall("SetActive", checked);
}

void AlarmWidget::contextMenuEvent(QContextMenuEvent* event) {
    QMenu* menu = new QMenu();
    menu->addSection(tr("For this alarm"));
    menu->addAction(QIcon::fromTheme("edit-rename"), tr("Edit"), this, [=] {
        AlarmPopover* add = new AlarmPopover(d->objectPath, this);
        tPopover* popover = new tPopover(add);
        popover->setPopoverWidth(SC_DPI(400));
        connect(add, &AlarmPopover::done, popover, &tPopover::dismiss);
        connect(popover, &tPopover::dismissed, popover, &tPopover::deleteLater);
        connect(popover, &tPopover::dismissed, add, &AlarmPopover::deleteLater);
        popover->show(this->window());
    });
    menu->addAction(QIcon::fromTheme("list-remove"), tr("Remove"), this, [=] {
        d->interface->asyncCall("Remove");
    });

    connect(menu, &QMenu::aboutToHide, menu, &QMenu::deleteLater);
    menu->popup(event->globalPos());
}
