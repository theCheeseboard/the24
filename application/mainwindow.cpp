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

#include <QMenu>
#include <QDesktopServices>
#include <QUrl>
#include <taboutdialog.h>
#include <tcsdtools.h>

struct MainWindowPrivate {
    tCsdTools csd;
};

MainWindow::MainWindow(QWidget* parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow) {
    ui->setupUi(this);

    d = new MainWindowPrivate();
    d->csd.installMoveAction(ui->topWidget);
    d->csd.installResizeAction(this);

    if (tCsdGlobal::windowControlsEdge() == tCsdGlobal::Left) {
        ui->leftCsdLayout->addWidget(d->csd.csdBoxForWidget(this));
    } else {
        ui->rightCsdLayout->addWidget(d->csd.csdBoxForWidget(this));
    }

    QMenu* menu = new QMenu(this);

    QMenu* helpMenu = new QMenu(this);
    helpMenu->setTitle(tr("Help"));
    helpMenu->addAction(ui->actionFileBug);
    helpMenu->addAction(ui->actionSources);
    helpMenu->addSeparator();
    helpMenu->addAction(ui->actionAbout);

    menu->addMenu(helpMenu);
    menu->addAction(ui->actionExit);

    ui->menuButton->setIconSize(SC_DPI_T(QSize(24, 24), QSize));
    ui->menuButton->setMenu(menu);
    ui->stackedWidget->setCurrentAnimation(tStackedWidget::SlideHorizontal);
}

MainWindow::~MainWindow() {
    delete d;
    delete ui;
}


void MainWindow::on_worldClockButton_toggled(bool checked) {
    if (checked) {
        ui->stackedWidget->setCurrentWidget(ui->worldClockPage);
    }
}

void MainWindow::on_alarmsButton_toggled(bool checked) {
    if (checked) {
        ui->stackedWidget->setCurrentWidget(ui->alarmsPage);
    }
}

void MainWindow::on_stopwatchButton_toggled(bool checked) {
    if (checked) {
        ui->stackedWidget->setCurrentWidget(ui->stopwatchPage);
    }
}

void MainWindow::on_timerButton_toggled(bool checked) {
    if (checked) {
        ui->stackedWidget->setCurrentWidget(ui->timerPage);
    }
}

void MainWindow::on_actionExit_triggered() {
    QApplication::exit();
}

void MainWindow::on_actionAbout_triggered() {
    tAboutDialog d;
    d.exec();
}

void MainWindow::on_actionSources_triggered() {
    QDesktopServices::openUrl(QUrl("http://github.com/vicr123/the24"));
}

void MainWindow::on_actionFileBug_triggered() {
    QDesktopServices::openUrl(QUrl("http://github.com/vicr123/the24/issues"));
}
