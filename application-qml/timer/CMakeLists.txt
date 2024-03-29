cntp_add_qml_module(timer the24-qml
    URI timer
    VERSION 1.0
    QML_FILES
        TimerPane.qml
        TimerDisplay.qml
        AddTimerPopover.qml
    SOURCES
        timermodel.h timermodel.cpp
        timerpanecontroller.h timerpanecontroller.cpp
)

target_link_libraries(the24-qml-qmlmodule-stopwatch PRIVATE the24-qml-common)
