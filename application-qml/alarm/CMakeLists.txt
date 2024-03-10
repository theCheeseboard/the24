qt_add_library(the24-qml-alarm STATIC)
qt_add_qml_module(the24-qml-alarm
    URI alarm
    VERSION 1.0
    QML_FILES
        AlarmsPane.qml
        SOURCES alarmmodel.h alarmmodel.cpp
        QML_FILES AlarmDisplay.qml
        SOURCES alarmpanecontroller.h alarmpanecontroller.cpp
        QML_FILES AddAlarmPopover.qml
)

target_link_libraries(the24-qml-alarm
    PRIVATE libcontemporary Qt::Quick Qt::QuickControls2
)
