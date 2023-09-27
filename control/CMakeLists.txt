cmake_minimum_required(VERSION 3.16)
project(control LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

find_package(QT NAMES Qt6 Qt5 REQUIRED COMPONENTS Core)
find_package(Qt${QT_VERSION_MAJOR} COMPONENTS Core Gui Widgets Network REQUIRED)

set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTORCC ON)
set(CMAKE_AUTOUIC ON)

set(PROJECT_SOURCES
    icon.qrc
    main.cpp
    Controller.h
    Controller.cpp
    MainWindow.ui
    MainWindow.h
    MainWindow.cpp
    CaptureDialog.ui
    CaptureDialog.h
    CaptureDialog.cpp
    ProtocolDialog.ui
    ProtocolDialog.h
    ProtocolDialog.cpp
    DomainListDialog.ui
    DomainListDialog.h
    DomainListDialog.cpp
    ServerListDialog.ui
    ServerListDialog.h
    ServerListDialog.cpp
)

add_executable(control ${PROJECT_SOURCES})

set_target_properties(control PROPERTIES WIN32_EXECUTABLE TRUE)

target_link_libraries(control PUBLIC Qt${QT_VERSION_MAJOR}::Core Qt${QT_VERSION_MAJOR}::Gui Qt${QT_VERSION_MAJOR}::Widgets Qt${QT_VERSION_MAJOR}::Network)
