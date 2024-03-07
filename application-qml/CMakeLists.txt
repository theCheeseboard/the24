cmake_minimum_required(VERSION 3.16)

set(CMAKE_CXX_STANDARD_REQUIRED ON)

find_package(libcontemporary)
find_package(Qt${QT_VERSION_MAJOR} REQUIRED COMPONENTS Quick QuickControls2 LinguistTools)

qt_standard_project_setup()

qt_add_executable(the24-qml
    main.cpp
)

qt_add_qml_module(the24-qml
    URI the24
    VERSION 1.0
    QML_FILES
        Main.qml
)

add_subdirectory(worldclock)

# Qt for iOS sets MACOSX_BUNDLE_GUI_IDENTIFIER automatically since Qt 6.1.
# If you are developing for iOS or macOS you should consider setting an
# explicit, fixed bundle identifier manually though.
set_target_properties(the24-qml PROPERTIES
#    MACOSX_BUNDLE_GUI_IDENTIFIER com.example.appplayground
    MACOSX_BUNDLE_BUNDLE_VERSION ${PROJECT_VERSION}
    MACOSX_BUNDLE_SHORT_VERSION_STRING ${PROJECT_VERSION_MAJOR}.${PROJECT_VERSION_MINOR}
    MACOSX_BUNDLE TRUE
    WIN32_EXECUTABLE TRUE
    QT_ANDROID_PACKAGE_SOURCE_DIR ${CMAKE_CURRENT_SOURCE_DIR}/android
    CNTP_DATA_SUBDIR the24
)

cntp_init(the24-qml 20)
cntp_translate(the24-qml)
cntp_target_name(the24-qml "the24")
# cntp_app_icon(the24-qml
    # BASEICON icons/baseicon.svg
# )
# cntp_target_desktop_file(the24-qml
    # DESKTOP_FILE com.vicr123.the24.desktop
#    BLUEPRINT_DESKTOP_FILE com.vicr123.the24_blueprint.desktop
# )
# cntp_defaults_file(the24-qml
    # DEFAULTS_FILE the24.conf)

target_link_libraries(the24-qml
    PRIVATE libcontemporary the24-qml-worldclock Qt::Quick Qt::QuickControls2
)

target_include_directories(the24-qml
    PRIVATE worldclock
)

if(CMAKE_SYSTEM_NAME STREQUAL "iOS")
    target_link_libraries(the24-qml
        PRIVATE contemporary-corecontrols-plugin contemporary-controls-plugin
    )
endif()

include(GNUInstallDirs)
install(TARGETS the24-qml
    BUNDLE DESTINATION .
    LIBRARY DESTINATION ${CMAKE_INSTALL_LIBDIR}
    RUNTIME DESTINATION ${CMAKE_INSTALL_BINDIR}
)
