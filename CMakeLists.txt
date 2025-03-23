cmake_minimum_required(VERSION 3.5)

add_subdirectory(SRendererCore/)

project(SRenderer VERSION 0.1 LANGUAGES CXX)

if(WIN32)
    set(assimp_DIR packages/assimp_x64-windows/share/assimp)
else()
    # disable qt keywords, to avoid qt keywords like "emit" conflict with TBB's emit
    add_definitions(-DQT_NO_KEYWORDS)
endif()

set(CMAKE_INCLUDE_CURRENT_DIR ON)

set(CMAKE_AUTOUIC ON)
set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTORCC ON)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

find_package(QT NAMES Qt6 Qt5 REQUIRED COMPONENTS Widgets)
find_package(Qt${QT_VERSION_MAJOR} REQUIRED COMPONENTS Widgets)

set(QRC_FILE
    qdarkstyle/dark/style.qrc
    qdarkstyle/light/style.qrc
    resource.qrc
)

set(PROJECT_SOURCES
        SRenderer.cpp
        SRenderer.h
        SRenderer.ui
        SRendererWidget.cpp
        SRendererWidget.h
        SRendererWidget.ui
        BlinnPhongShader.cpp
        BlinnPhongShader.h
        About.h
        About.cpp
        About.ui
        main.cpp
        Camera.h
        Camera.cpp
        Mesh.h
        Mesh.cpp
        Model.h
        Model.cpp
        ${QRC_FILE}
)

if(${QT_VERSION_MAJOR} GREATER_EQUAL 6)
    qt_add_executable(SRenderer
        MANUAL_FINALIZATION
        ${PROJECT_SOURCES}
    )
# Define target properties for Android with Qt 6 as:
#    set_property(TARGET SRenderer APPEND PROPERTY QT_ANDROID_PACKAGE_SOURCE_DIR
#                 ${CMAKE_CURRENT_SOURCE_DIR}/android)
# For more information, see https://doc.qt.io/qt-6/qt-add-executable.html#target-creation
else()
    if(ANDROID)
        add_library(SRenderer SHARED
            ${PROJECT_SOURCES}
        )
# Define properties for Android with Qt 5 after find_package() calls as:
#    set(ANDROID_PACKAGE_SOURCE_DIR "${CMAKE_CURRENT_SOURCE_DIR}/android")
    else()
        add_executable(SRenderer
            ${PROJECT_SOURCES}
        )
    endif()
endif()

#find_library(
#    ASSIMP
#    NAME assimp-vc142-mt
#    PATHS E:/lib/assimp/bin
#)
find_package(assimp CONFIG REQUIRED)

target_link_libraries(SRenderer PRIVATE Qt${QT_VERSION_MAJOR}::Widgets)
target_link_libraries(SRenderer PUBLIC SRendererCore)
target_link_libraries(SRenderer PRIVATE assimp::assimp)
target_include_directories(SRenderer PUBLIC
    SRendererCore/src
)

set_target_properties(SRenderer PROPERTIES
    MACOSX_BUNDLE_GUI_IDENTIFIER my.example.com
    MACOSX_BUNDLE_BUNDLE_VERSION ${PROJECT_VERSION}
    MACOSX_BUNDLE_SHORT_VERSION_STRING ${PROJECT_VERSION_MAJOR}.${PROJECT_VERSION_MINOR}
    MACOSX_BUNDLE TRUE
    WIN32_EXECUTABLE TRUE
)

if(QT_VERSION_MAJOR EQUAL 6)
    qt_finalize_executable(SRenderer)
endif()
