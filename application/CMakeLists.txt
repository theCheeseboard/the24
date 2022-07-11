project(the24 VERSION 1.0.0 LANGUAGES CXX)

find_package(Qt6 REQUIRED COMPONENTS Widgets)
find_package(libcontemporary REQUIRED)

set(SOURCES
    alarm/alarmpopover.cpp
    alarm/alarms.cpp
    alarm/alarmwidget.cpp
    clockhelpers.cpp
    main.cpp
    mainwindow.cpp
    stopwatch/stopwatch.cpp
    stopwatch/stopwatchwidget.cpp
    timer/newtimerpopover.cpp
    timer/timer.cpp
    timer/timerwidget.cpp
    worldclock/addclockpopover.cpp
    worldclock/analogueclock.cpp
    worldclock/timezonesmodel.cpp
    worldclock/worldclock.cpp
    worldclock/worldclockwidget.cpp

    alarm/alarmpopover.ui
    alarm/alarms.ui
    alarm/alarmwidget.ui
    mainwindow.ui
    stopwatch/stopwatch.ui
    stopwatch/stopwatchwidget.ui
    timer/newtimerpopover.ui
    timer/timer.ui
    timer/timerwidget.ui
    worldclock/addclockpopover.ui
    worldclock/analogueclock.ui
    worldclock/worldclock.ui
    worldclock/worldclockwidget.ui

    resources.qrc
)

set(HEADERS
    alarm/alarmpopover.h
    alarm/alarms.h
    alarm/alarmwidget.h
    clockhelpers.h
    mainwindow.h
    stopwatch/stopwatch.h
    stopwatch/stopwatchwidget.h
    timer/newtimerpopover.h
    timer/timer.h
    timer/timerwidget.h
    worldclock/addclockpopover.h
    worldclock/analogueclock.h
    worldclock/timezonesmodel.h
    worldclock/worldclock.h
    worldclock/worldclockwidget.h
)

add_executable(the24 ${SOURCES} ${HEADERS})

set_target_properties(the24 PROPERTIES
        MACOSX_BUNDLE TRUE
        MACOSX_BUNDLE_INFO_PLIST ${CMAKE_CURRENT_SOURCE_DIR}/Info.plist.in
        MACOSX_BUNDLE_GUI_IDENTIFIER com.vicr123.the24
        WIN32_EXECUTABLE TRUE
        CNTP_DATA_SUBDIR the24)

cntp_init(the24 20)
cntp_translate(the24)
cntp_target_name(the24 "the24")
#cntp_app_icon(the24
#    BASEICON icons/baseicon.svg
#    BASEICON_MAC icons/baseicon-mac.svg)
cntp_target_desktop_file(the24
    DESKTOP_FILE com.vicr123.the24.desktop
#    BLUEPRINT_DESKTOP_FILE com.vicr123.the24_blueprint.desktop
)
cntp_defaults_file(the24
    DEFAULTS_FILE the24.conf)

target_link_libraries(the24 Qt6::Widgets libcontemporary)

install(TARGETS the24
        RUNTIME DESTINATION ${CMAKE_INSTALL_BINDIR}
        BUNDLE DESTINATION /Applications)
