cmake_minimum_required(VERSION 3.8)
project(control LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

find_package(QT NAMES Qt6 Qt5 REQUIRED COMPONENTS Core)
find_package(Qt${QT_VERSION_MAJOR} COMPONENTS Core Gui Widgets Network REQUIRED)

set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTORCC ON)
set(CMAKE_AUTOUIC ON)

set(PROJECT_SOURCES
    app_ico.rc
    icon.qrc
    main.cpp
    QHexView/document/buffer/qfilebuffer.cpp
    QHexView/document/buffer/qhexbuffer.cpp
    QHexView/document/buffer/qmemorybuffer.cpp
    QHexView/document/buffer/qmemoryrefbuffer.cpp
    QHexView/document/commands/hexcommand.cpp
    QHexView/document/commands/insertcommand.cpp
    QHexView/document/commands/removecommand.cpp
    QHexView/document/commands/replacecommand.cpp
    QHexView/document/qhexcursor.cpp
    QHexView/document/qhexdocument.cpp
    QHexView/document/qhexmetadata.cpp
    QHexView/document/qhexrenderer.cpp
    QHexView/qhexview.cpp
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
    DeviceListDialog.ui
    DeviceListDialog.h
    DeviceListDialog.cpp
)

add_executable(control ${PROJECT_SOURCES})

set_target_properties(control PROPERTIES WIN32_EXECUTABLE TRUE)

target_link_libraries(control PUBLIC Qt${QT_VERSION_MAJOR}::Core Qt${QT_VERSION_MAJOR}::Gui Qt${QT_VERSION_MAJOR}::Widgets Qt${QT_VERSION_MAJOR}::Network)
