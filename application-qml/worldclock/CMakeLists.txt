qt_add_library(the24-qml-worldclock STATIC)
qt_add_qml_module(the24-qml-worldclock
    URI worldclock
    VERSION 1.0
    QML_FILES
        WorldClocksPane.qml
    SOURCES
        worldclockcontroller.h worldclockcontroller.cpp
        worldclock.h worldclock.cpp
)

target_link_libraries(the24-qml-worldclock
    PRIVATE libcontemporary Qt::Quick Qt::QuickControls2
)
