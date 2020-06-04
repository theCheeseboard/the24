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
#include <QCoreApplication>

#include <QDBusConnection>
#include <QSqlDatabase>
#include <QSqlQuery>
#include <QDir>
#include <QStandardPaths>
#include <QDebug>
#include <tsettings.h>
#include "the24manager.h"

int main(int argc, char* argv[]) {
    QCoreApplication a(argc, argv);

    a.setApplicationName("the24");
    a.setOrganizationName("theSuite");

    tSettings::registerDefaults(a.applicationDirPath() + "../application/defaults.conf");
    tSettings::registerDefaults("/etc/theSuite/the24/defaults.conf");

    QString dataPath = QStandardPaths::writableLocation(QStandardPaths::AppDataLocation);
    QDir::root().mkpath(dataPath);
    QString dbPath = QDir(dataPath).absoluteFilePath("clocks.db");
    QSqlDatabase db = QSqlDatabase::addDatabase("QSQLITE");
    if (!db.isValid()) {
        //Couldn't open the database, so bail out here
        qWarning() << "Couldn't open database";
        return 1;
    }
    db.setDatabaseName(dbPath);
    if (!db.open()) {
        //Couldn't open the database, so bail out here
        qWarning() << "Couldn't open database";
        return 1;
    }

    //Initialise the tables
    QStringList tables = db.tables();
    if (!tables.contains("TIMERS")) {
        db.exec("CREATE TABLE timers(id INTEGER PRIMARY KEY, timeout INTEGER, length INTEGER, pausedRemaining INTEGER DEFAULT NULL, note TEXT DEFAULT NULL)");
    }

    if (!QDBusConnection::sessionBus().registerService("com.vicr123.the24")) {
        //Couldn't register the service, so bail out here
        qWarning() << "Couldn't register D-Bus service";
        return 1;
    }

    The24Manager* mgr = new The24Manager();

    return a.exec();
}
