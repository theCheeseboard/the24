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
#ifndef CLOCKHELPERS_H
#define CLOCKHELPERS_H

#include <QCoreApplication>
#include <QString>

class ClockHelpers {
        Q_DECLARE_TR_FUNCTIONS(ClockHelpers)

    public:
        static QString msecsToString(qlonglong msecs);
        static QString msecsToStringWithMsecs(qlonglong msecs);

    private:
        ClockHelpers() = delete;
};

#endif // CLOCKHELPERS_H
