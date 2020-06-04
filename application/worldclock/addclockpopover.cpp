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
#include "addclockpopover.h"
#include "ui_addclockpopover.h"

#include "timezonesmodel.h"

struct AddClockPopoverPrivate {
    TimezonesModel* model;
};

AddClockPopover::AddClockPopover(QWidget* parent) :
    QWidget(parent),
    ui(new Ui::AddClockPopover) {
    ui->setupUi(this);

    d = new AddClockPopoverPrivate();
    ui->titleLabel->setBackButtonShown(true);

    d->model = new TimezonesModel(this);
    ui->timezoneList->setModel(d->model);
    ui->timezoneList->setItemDelegate(new TimezonesModelDelegate(this));
}

AddClockPopover::~AddClockPopover() {
    delete d;
    delete ui;
}

void AddClockPopover::on_titleLabel_backButtonClicked() {
    emit done();
}

void AddClockPopover::on_searchLineEdit_textChanged(const QString& arg1) {
    d->model->search(arg1);
}

void AddClockPopover::on_timezoneList_activated(const QModelIndex& index) {
    emit addClock(index.data(Qt::UserRole).toByteArray());
    emit done();
}
