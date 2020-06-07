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
#ifndef THE24MANAGER_H
#define THE24MANAGER_H

#include <QObject>
#include <QDBusObjectPath>

struct The24ManagerPrivate;
class The24Manager : public QObject {
        Q_OBJECT
        Q_CLASSINFO("D-Bus Interface", "com.vicr123.the24")

    public:
        explicit The24Manager(QObject* parent = nullptr);
        ~The24Manager();

    public Q_SLOTS:
        Q_SCRIPTABLE QStringList EnumerateAlarms();
        Q_SCRIPTABLE QStringList EnumerateTimers();
        Q_SCRIPTABLE QStringList EnumerateStopwatches();

        Q_SCRIPTABLE QString AddTimer(qint64 msecsFromNow);
        Q_SCRIPTABLE QString AddStopwatch();

        Q_SCRIPTABLE void RequestExit();

    Q_SIGNALS:
        Q_SCRIPTABLE void TimerAdded(QString objectPath);
        Q_SCRIPTABLE void TimerRemoved(QString objectPath);
        Q_SCRIPTABLE void StopwatchAdded(QString objectPath);
        Q_SCRIPTABLE void StopwatchRemoved(QString objectPath);

    private:
        The24ManagerPrivate* d;
};

#endif // THE24MANAGER_H
