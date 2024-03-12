cntp_add_qml_module(worldclock the24-qml
    URI worldclock
    VERSION 1.0
    QML_FILES
        WorldClocksPane.qml
        WorldClockDisplay.qml
        AddClockPopup.qml
    SOURCES
        worldclockcontroller.h worldclockcontroller.cpp
        worldclock.h worldclock.cpp
        timezonesmodel.h timezonesmodel.cpp
        SOURCES analogueclock.h analogueclock.cpp
)
