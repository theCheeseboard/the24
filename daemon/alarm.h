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
#ifndef ALARM_H
#define ALARM_H

#include <QObject>

struct AlarmPrivate;
class Alarm : public QObject {
        Q_OBJECT
        Q_CLASSINFO("D-Bus Interface", "com.vicr123.the24.Alarm")

    public:
        explicit Alarm(int id, QObject* parent = nullptr);
        ~Alarm();

        QString objectPath();
        void tick();

    public Q_SLOTS:
        Q_SCRIPTABLE void Remove();
        Q_SCRIPTABLE bool Active();
        Q_SCRIPTABLE qulonglong Offset();
        Q_SCRIPTABLE qulonglong Repeat();
        Q_SCRIPTABLE qlonglong SnoozeOffset();
        Q_SCRIPTABLE bool WillIgnoreNext();

        Q_SCRIPTABLE void SetActive(bool active);
        Q_SCRIPTABLE void SetOffset(qulonglong offset);
        Q_SCRIPTABLE void SetRepeat(qulonglong repeat);
        Q_SCRIPTABLE void IgnoreNext();

    signals:
        Q_SCRIPTABLE void Removing();
        Q_SCRIPTABLE void ActiveChanged(bool active);
        Q_SCRIPTABLE void OffsetChanged(qulonglong offset);
        Q_SCRIPTABLE void SnoozeOffsetChanged(qlonglong offset);
        Q_SCRIPTABLE void RepeatChanged(qulonglong repeat);
        Q_SCRIPTABLE void IgnoringNextChanged(bool ignoring);

    private:
        AlarmPrivate* d;

        void updateFromDatabase();
        void writeToDatabase();
        void prepareTone();
};

#endif // ALARM_H
