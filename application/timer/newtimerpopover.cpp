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
#include "newtimerpopover.h"
#include "ui_newtimerpopover.h"

#include <QDBusMessage>
#include <QDBusConnection>

#include <tdatetimepicker.h>

struct NewTimerPopoverPrivate {
    tDateTimePicker* picker;
};

NewTimerPopover::NewTimerPopover(QWidget* parent) :
    QWidget(parent),
    ui(new Ui::NewTimerPopover) {
    ui->setupUi(this);

    d = new NewTimerPopoverPrivate;

    ui->titleLabel->setBackButtonShown(true);

    d->picker = new tDateTimePicker("Hms", this);
    ui->pickerLayout->addWidget(d->picker);

    QFont fnt = d->picker->font();
    fnt.setPointSize(30);
    d->picker->setFont(fnt);

    d->picker->setDateTime(QDateTime(QDate(), QTime(0, 5, 0)));
    d->picker->setPickOptions(tDateTimePicker::PickTime);
}

NewTimerPopover::~NewTimerPopover() {
    delete d;
    delete ui;
}

void NewTimerPopover::on_titleLabel_backButtonClicked() {
    emit done();
}

void NewTimerPopover::on_startButton_clicked() {
    QDBusMessage message = QDBusMessage::createMethodCall("com.vicr123.the24", "/com/vicr123/the24", "com.vicr123.the24", "AddTimer");
    message.setArguments({
        static_cast<qint64>(d->picker->currentDateTime().time().msecsSinceStartOfDay())
    });
    QDBusConnection::sessionBus().asyncCall(message);
    emit done();
}
