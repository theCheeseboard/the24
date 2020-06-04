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
#ifndef TIMER_H
#define TIMER_H

#include <QWidget>

namespace Ui {
    class Timer;
}

struct TimerPrivate;
class Timer : public QWidget {
        Q_OBJECT

    public:
        explicit Timer(QWidget* parent = nullptr);
        ~Timer();

    public slots:
        void addTimer(QString path);

    private slots:
        void on_addButton_clicked();

    private:
        Ui::Timer* ui;
        TimerPrivate* d;

        void serviceAvailable();
        void serviceUnavailable();
};

#endif // TIMER_H
