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
#ifndef STOPWATCHWIDGET_H
#define STOPWATCHWIDGET_H

#include <QWidget>
#include <Task>

namespace Ui {
    class StopwatchWidget;
}

struct StopwatchWidgetPrivate;
class StopwatchWidget : public QWidget {
        Q_OBJECT

    public:
        explicit StopwatchWidget(QString objectPath, QWidget* parent = nullptr);
        ~StopwatchWidget();

    private slots:
        QCoro::Task<> update();

        void on_removeButton_clicked();

        void on_resetButton_clicked();

        void on_actionButton_clicked();

    private:
        Ui::StopwatchWidget* ui;
        StopwatchWidgetPrivate* d;

        void updateDisplay();
};

#endif // STOPWATCHWIDGET_H
