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
#include "clockhelpers.h"

#include <QApplication>

QString ClockHelpers::msecsToString(qlonglong msecs) {
    QString duration;
    QLocale locale;

    qulonglong secs = msecs / 1000;
    qulonglong mins = secs / 60;
    qulonglong hours = mins / 60;
    qulonglong days = hours / 24;

    hours %= 24;
    mins %= 60;
    secs %= 60;

    if (days > 0) {
        duration = QStringLiteral("%1%2 %3:%4:%5").arg(days).arg(tr("d", "d for \"Day\" (1d)"));
    } else {
        duration = QStringLiteral("%1:%2:%3");
    }

    duration = duration.arg(hours, 2, 10, locale.zeroDigit()).arg(mins, 2, 10, locale.zeroDigit()).arg(secs, 2, 10, locale.zeroDigit());

    return duration;
}

QString ClockHelpers::msecsToStringWithMsecs(qlonglong msecs) {
    QString duration = msecsToString(msecs);
    duration += "." + QStringLiteral("%1").arg(msecs % 1000, 3, 10, QLocale().zeroDigit());
    return duration;
}
