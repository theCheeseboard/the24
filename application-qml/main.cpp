#include <QDBusConnection>
#include <QDBusConnectionInterface>
#include <QGuiApplication>
#include <QIcon>
#include <QProcess>
#include <QQmlApplicationEngine>
#include <QQmlEngine>
#include <QQmlExtensionPlugin>
#include <QQuickStyle>
#include <tapplication.h>
#include <tsettings.h>

int main(int argc, char* argv[]) {
    tApplication a(argc, argv);
    a.setWindowIcon(QIcon::fromTheme("com.vicr123.the24"));
    a.setApplicationShareDir("the24");
    a.installTranslators();

    a.setApplicationVersion("3.0");
    a.setGenericName(QApplication::translate("main", "Clock"));
    a.setApplicationLicense(tApplication::Gpl3OrLater);
    a.setCopyrightHolder("Victor Tran");
    a.setCopyrightYear("2024");
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

    QQuickStyle::setStyle("com.vicr123.Contemporary.CoreStyles");

    QQmlApplicationEngine engine;
    const QUrl url(u"qrc:/qt/qml/the24/Main.qml"_qs);
    QObject::connect(
        &engine,
        &QQmlApplicationEngine::objectCreationFailed,
        &a,
        []() {
        QCoreApplication::exit(-1);
    },
        Qt::QueuedConnection);
    engine.load(url);

    return a.exec();
}

#ifdef Q_OS_IOS
Q_IMPORT_QML_PLUGIN(com_vicr123_Contemporary_CoreStylesPlugin)
#endif
