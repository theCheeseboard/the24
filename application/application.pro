QT       += core gui thelib
SHARE_APP_NAME = the24

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

CONFIG += c++11
TARGET = the24

# Include the-libs build tools
include(/usr/share/the-libs/pri/buildmaster.pri)

# The following define makes your compiler emit warnings if you use
# any Qt feature that has been marked deprecated (the exact warnings
# depend on your compiler). Please consult the documentation of the
# deprecated API in order to know how to port your code away from it.
DEFINES += QT_DEPRECATED_WARNINGS

# You can also make your code fail to compile if it uses deprecated APIs.
# In order to do so, uncomment the following line.
# You can also select to disable deprecated APIs only up to a certain version of Qt.
#DEFINES += QT_DISABLE_DEPRECATED_BEFORE=0x060000    # disables all the APIs deprecated before Qt 6.0.0

QMAKE_POST_LINK += $$QMAKE_COPY_DIR $$quote($$PWD/translations) $$shell_quote($$OUT_PWD) && \
    $$QMAKE_COPY $$quote($$PWD/defaults.conf) $$shell_quote($$OUT_PWD)

SOURCES += \
    alarm/alarmpopover.cpp \
    alarm/alarms.cpp \
    alarm/alarmwidget.cpp \
    clockhelpers.cpp \
    main.cpp \
    mainwindow.cpp \
    stopwatch/stopwatch.cpp \
    stopwatch/stopwatchwidget.cpp \
    timer/newtimerpopover.cpp \
    timer/timer.cpp \
    timer/timerwidget.cpp \
    worldclock/addclockpopover.cpp \
    worldclock/timezonesmodel.cpp \
    worldclock/worldclock.cpp \
    worldclock/worldclockwidget.cpp

HEADERS += \
    alarm/alarmpopover.h \
    alarm/alarms.h \
    alarm/alarmwidget.h \
    clockhelpers.h \
    mainwindow.h \
    stopwatch/stopwatch.h \
    stopwatch/stopwatchwidget.h \
    timer/newtimerpopover.h \
    timer/timer.h \
    timer/timerwidget.h \
    worldclock/addclockpopover.h \
    worldclock/timezonesmodel.h \
    worldclock/worldclock.h \
    worldclock/worldclockwidget.h

FORMS += \
    alarm/alarmpopover.ui \
    alarm/alarms.ui \
    alarm/alarmwidget.ui \
    mainwindow.ui \
    stopwatch/stopwatch.ui \
    stopwatch/stopwatchwidget.ui \
    timer/newtimerpopover.ui \
    timer/timer.ui \
    timer/timerwidget.ui \
    worldclock/addclockpopover.ui \
    worldclock/worldclock.ui \
    worldclock/worldclockwidget.ui

unix {
    target.path = /usr/bin/

    defaults.files = defaults.conf
    defaults.path = /etc/theSuite/the24/

    desktop.path = /usr/share/applications
    desktop.files = com.vicr123.the24.desktop

    icon.path = /usr/share/icons/hicolor/scalable/apps/
    icon.files = icons/the24.svg

    INSTALLS += target defaults icon desktop
}

RESOURCES += \
    resources.qrc

DISTFILES += \
    com.vicr123.the24.desktop \
    defaults.conf
