cntp_add_static_module(
    MODULE_NAME common
    PARENT_TARGET the24-qml
    TARGET_NAME the24-qml-common
    SOURCES
        clockhelpers.cpp clockhelpers.h
)

target_link_libraries(the24-qml-common PRIVATE Qt::Core Qt::Widgets)
