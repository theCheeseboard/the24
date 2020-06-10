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
#include "alarmpopover.h"
#include "ui_alarmpopover.h"

#include <QDBusInterface>
#include <QPushButton>
#include <tdatetimepicker.h>

struct AlarmPopoverPrivate {
    tDateTimePicker* picker;
    qulonglong repeat = 0;

    QDBusInterface* interface = nullptr;
};

AlarmPopover::AlarmPopover(QString objectPath, QWidget* parent) :
    QWidget(parent),
    ui(new Ui::AlarmPopover) {
    ui->setupUi(this);

    d = new AlarmPopoverPrivate();

    QTime time = QTime::currentTime();
    if (!objectPath.isEmpty()) {
        d->interface = new QDBusInterface("com.vicr123.the24", objectPath, "com.vicr123.the24.Alarm", QDBusConnection::sessionBus(), this);
        d->repeat = d->interface->call("Repeat").arguments().first().toULongLong();
        time = QTime::fromMSecsSinceStartOfDay(d->interface->call("Offset").arguments().first().toULongLong());
        ui->addButton->setVisible(false);
        ui->titleLabel->setText(tr("Edit Alarm"));
    } else {
        ui->titleLabel->setText(tr("New Alarm"));
    }

    ui->titleLabel->setBackButtonShown(true);

    d->picker = new tDateTimePicker("hma", this);
    ui->pickerLayout->addWidget(d->picker);

    QFont fnt = d->picker->font();
    fnt.setPointSize(30);
    d->picker->setFont(fnt);

    d->picker->setDateTime(QDateTime(QDate::currentDate(), time));
    d->picker->setPickOptions(tDateTimePicker::PickTime);

    QLocale l;
    for (int i = l.firstDayOfWeek() - 1; i < l.firstDayOfWeek() + 6; i++) {
        int day = i % 7;

        QPushButton* btn = new QPushButton(this);
        btn->setText(l.dayName(day + 1, QLocale::NarrowFormat));
        btn->setCheckable(true);
        btn->setChecked(d->repeat & 1 << day);
        connect(btn, &QPushButton::toggled, this, [ = ](bool checked) {
            if (checked) {
                d->repeat |= 1 << day;
            } else {
                d->repeat &= ~(1 << day);
            }

            if (d->interface) {
                d->interface->call("SetRepeat", d->repeat);
            }
        });
        ui->repeatLayout->addWidget(btn);
//        if (repeats & 1 << day) repeatsList.append(l.dayName(day + 1, QLocale::ShortFormat));
    }

    connect(d->picker, &tDateTimePicker::dateTimeChanged, this, [ = ](QDateTime dateTime) {
        if (d->interface) {
            d->interface->asyncCall("SetOffset", static_cast<qulonglong>(dateTime.time().msecsSinceStartOfDay()));
        }
    });
}

AlarmPopover::~AlarmPopover() {
    delete d;
    delete ui;
}

void AlarmPopover::on_titleLabel_backButtonClicked() {
    emit done();
}

void AlarmPopover::on_addButton_clicked() {
    QDBusMessage message = QDBusMessage::createMethodCall("com.vicr123.the24", "/com/vicr123/the24", "com.vicr123.the24", "AddAlarm");
    message.setArguments({
        static_cast<qulonglong>(d->picker->currentDateTime().time().msecsSinceStartOfDay()),
        d->repeat
    });
    QDBusConnection::sessionBus().asyncCall(message);
    emit done();
}
