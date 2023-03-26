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
#include "mainwindow.h"
#include "ui_mainwindow.h"

#include <QDesktopServices>
#include <QMenu>
#include <QUrl>
#include <tapplication.h>
#include <tcsdtools.h>
#include <thelpmenu.h>

struct MainWindowPrivate {
        tCsdTools csd;
};

MainWindow::MainWindow(QWidget* parent) :
    QMainWindow(parent), ui(new Ui::MainWindow) {
    ui->setupUi(this);

    d = new MainWindowPrivate();
    d->csd.installMoveAction(ui->topWidget);
    d->csd.installResizeAction(this);

    if (tCsdGlobal::windowControlsEdge() == tCsdGlobal::Left) {
        ui->leftCsdLayout->addWidget(d->csd.csdBoxForWidget(this));
    } else {
        ui->rightCsdLayout->addWidget(d->csd.csdBoxForWidget(this));
    }

    this->setWindowIcon(tApplication::applicationIcon());

    QMenu* menu = new QMenu(this);
    menu->addMenu(new tHelpMenu(this));
    menu->addAction(ui->actionExit);

    ui->menuButton->setIconSize(SC_DPI_T(QSize(24, 24), QSize));
    ui->menuButton->setIcon(tApplication::applicationIcon());
    ui->menuButton->setMenu(menu);
    ui->stackedWidget->setCurrentAnimation(tStackedWidget::SlideHorizontal);

    ui->windowTabber->addButton(new tWindowTabberButton(QIcon::fromTheme("clock-world"), tr("World Clock"), ui->stackedWidget, ui->worldClockPage));
    ui->windowTabber->addButton(new tWindowTabberButton(QIcon::fromTheme("clock-alarm"), tr("Alarms"), ui->stackedWidget, ui->alarmsPage));
    ui->windowTabber->addButton(new tWindowTabberButton(QIcon::fromTheme("clock-stopwatch"), tr("Stopwatch"), ui->stackedWidget, ui->stopwatchPage));
    ui->windowTabber->addButton(new tWindowTabberButton(QIcon::fromTheme("clock-timer"), tr("Timer"), ui->stackedWidget, ui->timerPage));
}

MainWindow::~MainWindow() {
    delete d;
    delete ui;
}

void MainWindow::on_actionExit_triggered() {
    QApplication::exit();
}
