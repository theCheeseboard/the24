cntp_add_qml_module(alarm the24-qml
    URI alarm
    VERSION 1.0
    QML_FILES
        AlarmsPane.qml
        SOURCES alarmmodel.h alarmmodel.cpp
        QML_FILES AlarmDisplay.qml
        SOURCES alarmpanecontroller.h alarmpanecontroller.cpp
        QML_FILES AddAlarmPopover.qml
)
