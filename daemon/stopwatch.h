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
#ifndef STOPWATCH_H
#define STOPWATCH_H

#include <QObject>
#include <QDBusContext>

struct StopwatchPrivate;
class Stopwatch : public QObject, protected QDBusContext {
        Q_OBJECT
        Q_CLASSINFO("D-Bus Interface", "com.vicr123.the24.Stopwatch")

    public:
        explicit Stopwatch(int id, QObject* parent = nullptr);
        ~Stopwatch();

        QString objectPath();

    public Q_SLOTS:
        Q_SCRIPTABLE void Remove();
        Q_SCRIPTABLE qulonglong Elapsed();
        Q_SCRIPTABLE bool Paused();

        Q_SCRIPTABLE void Start();
        Q_SCRIPTABLE void Stop();
        Q_SCRIPTABLE void Reset();
        Q_SCRIPTABLE void Lap();

        Q_SCRIPTABLE QList<qulonglong> Laps();

    signals:
        Q_SCRIPTABLE void Removing();
        Q_SCRIPTABLE void LapsChanged();
        Q_SCRIPTABLE void StateChanged();

    private:
        StopwatchPrivate* d;

        void updateFromDatabase();
        void updateRunningValues();
        void writeToDatabase();
};

#endif // STOPWATCH_H
