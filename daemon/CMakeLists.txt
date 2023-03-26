project(the24d VERSION 1.0.0 LANGUAGES CXX)

find_package(Qt${QT_VERSION_MAJOR} REQUIRED COMPONENTS Sql)
find_package(libcontemporary REQUIRED)

set(SOURCES
    alarm.cpp
    main.cpp
    stopwatch.cpp
    the24manager.cpp
    timer.cpp
)

set(HEADERS
    alarm.h
    stopwatch.h
    the24manager.h
    timer.h
)

qt_add_dbus_adaptor(SOURCES com.vicr123.the24.xml the24manager.h The24Manager)
qt_add_dbus_adaptor(SOURCES com.vicr123.the24.Timer.xml timer.h Timer)
qt_add_dbus_adaptor(SOURCES com.vicr123.the24.Stopwatch.xml stopwatch.h Stopwatch)
qt_add_dbus_adaptor(SOURCES com.vicr123.the24.Alarm.xml alarm.h Alarm)

add_executable(the24d ${SOURCES} ${HEADERS})

set_target_properties(the24d PROPERTIES
        CNTP_DATA_SUBDIR the24/daemon)

cntp_init(the24d 20)
cntp_translate(the24d)
cntp_target_name(the24d "the24d")

target_link_libraries(the24d Qt::Sql libcontemporary)

install(TARGETS the24d
        RUNTIME DESTINATION ${CMAKE_INSTALL_BINDIR}
        BUNDLE DESTINATION /Applications)

install(FILES com.vicr123.the24.daemon.desktop
    DESTINATION ${CMAKE_INSTALL_SYSCONFDIR}/xdg/autostart)

install(DIRECTORY tones
    DESTINATION ${CMAKE_INSTALL_DATADIR}/sounds/the24)
