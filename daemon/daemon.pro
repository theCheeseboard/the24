QT += gui widgets dbus thelib sql multimedia
SHARE_APP_NAME = the24/daemon

CONFIG += c++11 console
CONFIG -= app_bundle
TARGET = the24d

# Include the-libs build tools
include(/usr/share/the-libs/pri/buildmaster.pri)

DBUS_ADAPTORS += com.vicr123.the24.xml \
    com.vicr123.the24.Timer.xml \
    com.vicr123.the24.Stopwatch.xml

# The following define makes your compiler emit warnings if you use
# any Qt feature that has been marked deprecated (the exact warnings
# depend on your compiler). Please consult the documentation of the
# deprecated API in order to know how to port your code away from it.
DEFINES += QT_DEPRECATED_WARNINGS

# You can also make your code fail to compile if it uses deprecated APIs.
# In order to do so, uncomment the following line.
# You can also select to disable deprecated APIs only up to a certain version of Qt.
#DEFINES += QT_DISABLE_DEPRECATED_BEFORE=0x060000    # disables all the APIs deprecated before Qt 6.0.0

SOURCES += \
        main.cpp \
        stopwatch.cpp \
        the24manager.cpp \
        timer.cpp

unix {
    target.path = /usr/bin/

    tones.files = tones
    tones.path = /usr/share/sounds/the24/

    INSTALLS += target tones
}

HEADERS += \
    stopwatch.h \
    the24manager.h \
    timer.h

RESOURCES +=
