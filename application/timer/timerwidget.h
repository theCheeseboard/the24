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
#ifndef TIMERWIDGET_H
#define TIMERWIDGET_H

#include <QWidget>

namespace Ui {
    class TimerWidget;
}

struct TimerWidgetPrivate;
class TimerWidget : public QWidget {
        Q_OBJECT

    public:
        explicit TimerWidget(QString objectPath, QWidget* parent = nullptr);
        ~TimerWidget();

    private slots:
        void on_actionButton_clicked();

        void update();

        void on_resetButton_clicked();

        void on_removeButton_clicked();

    private:
        Ui::TimerWidget* ui;
        TimerWidgetPrivate* d;

        void updateDisplay();
};

#endif // TIMERWIDGET_H
