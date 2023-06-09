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
#include "mainwindow.h"

#include <QDir>
#include <tapplication.h>
#include <tsettings.h>

#include <QDBusConnection>
#include <QDBusConnectionInterface>
#include <QProcess>

int main(int argc, char* argv[]) {
    tApplication a(argc, argv);
    a.setApplicationShareDir("the24");
    a.installTranslators();

    a.setApplicationVersion("2.0");
    a.setGenericName(QApplication::translate("main", "Clock"));
    a.setApplicationLicense(tApplication::Gpl3OrLater);
    a.setCopyrightHolder("Victor Tran");
    a.setCopyrightYear("2023");
    a.setOrganizationName("theSuite");
    a.setApplicationName(T_APPMETA_READABLE_NAME);
    a.setDesktopFileName(T_APPMETA_DESKTOP_ID);
    a.setApplicationUrl(tApplication::HelpContents, QUrl("http://help.vicr123.com/docs/the24/intro"));
    a.setApplicationUrl(tApplication::Sources, QUrl("http://github.com/vicr123/the24"));
    a.setApplicationUrl(tApplication::FileBug, QUrl("http://github.com/vicr123/the24/issues"));

    tSettings::registerDefaults(a.applicationDirPath() + "/defaults.conf");
    tSettings::registerDefaults("/etc/theSuite/the24/defaults.conf");

    if (!QDBusConnection::sessionBus().interface()->isServiceRegistered("com.vicr123.the24").value()) {
        QProcess::startDetached("the24d", {});
    }

    MainWindow w;
    w.show();

    return a.exec();
}
