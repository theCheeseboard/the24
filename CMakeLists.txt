cmake_minimum_required(VERSION 3.1.0)
set(CMAKE_OSX_DEPLOYMENT_TARGET 10.15)

project(the24 VERSION 1.0.0 LANGUAGES CXX)
set(CMAKE_CXX_STANDARD 20)

IF(${CMAKE_SYSTEM_NAME} MATCHES "Darwin")
    link_directories(/usr/local/lib)
ENDIF()

find_package(QT REQUIRED NAMES Qt6)
find_package(Qt${QT_VERSION_MAJOR} REQUIRED COMPONENTS Quick QuickControls2)

qt_policy(
    SET QTP0001 NEW
)

add_subdirectory(application-qml)
add_subdirectory(application)
add_subdirectory(daemon)
