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

#include <QObject>

struct TimerPrivate;
class Timer : public QObject {
        Q_OBJECT
        Q_CLASSINFO("D-Bus Interface", "com.vicr123.the24.Timer")
    public:
        explicit Timer(int id, QObject* parent = nullptr);
        ~Timer();

        QString objectPath();

    public Q_SLOTS:
        Q_SCRIPTABLE void Remove();
        Q_SCRIPTABLE qlonglong MillisecondsRemaining();
        Q_SCRIPTABLE void SetLength(quint64 length);
        Q_SCRIPTABLE void SetNote(QString note);

        Q_SCRIPTABLE void Pause();
        Q_SCRIPTABLE void Resume();
        Q_SCRIPTABLE void Reset();
        Q_SCRIPTABLE QString State();
        Q_SCRIPTABLE QString Note();

    signals:
        Q_SCRIPTABLE void Timeout();
        Q_SCRIPTABLE void StateChanged();
        Q_SCRIPTABLE void LengthChanged();
        Q_SCRIPTABLE void NoteChanged();
        Q_SCRIPTABLE void Removing();

    private:
        TimerPrivate* d;

        void updateFromDatabase();
        void writeToDatabase();
        void doTimeout();
        void prepareTone();
};

#endif // TIMER_H
