cntp_add_qml_module(stopwatch the24-qml
    URI stopwatch
    VERSION 1.0
    QML_FILES
        StopwatchPane.qml
        StopwatchDisplay.qml
    SOURCES
        stopwatchmodel.h stopwatchmodel.cpp
        stopwatchpanecontroller.h stopwatchpanecontroller.cpp
)

target_link_libraries(the24-qml-qmlmodule-stopwatch PRIVATE the24-qml-common)
