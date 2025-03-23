cmake_minimum_required(VERSION 3.5)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED True)
set(CMAKE_DEBUG_POSTFIX "d")

if(WIN32)
    set(glm_DIR ../packages/glm_x64-windows/share/glm)
    set(TBB_DIR ../packages/tbb_x64-windows/share/tbb)
endif()
project(SRendererCore LANGUAGES CXX)

set(CMAKE_INCLUDE_CURRENT_DIR ON)
set(CMAKE_AUTOUIC ON)
set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTORCC ON)
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

find_package(QT NAMES Qt6 Qt5 REQUIRED COMPONENTS Widgets)
find_package(Qt${QT_VERSION_MAJOR} REQUIRED COMPONENTS Widgets)

set(SRENDERERCORE_SRC
    src/BasicDataStructure.hpp
    src/FrameBuffer.cpp
    src/FrameBuffer.h
    src/Shader.hpp
    src/SRendererCoreExport.h
    src/SRendererDevice.cpp
    src/SRendererDevice.h
    src/Texture.h
    src/Texture.cpp
)

find_package(glm CONFIG REQUIRED)
find_package(TBB REQUIRED)

add_library(SRendererCore SHARED ${SRENDERERCORE_SRC})

target_link_libraries(SRendererCore PRIVATE Qt${QT_VERSION_MAJOR}::Widgets)
target_link_libraries(SRendererCore PUBLIC glm::glm)
target_link_libraries(SRendererCore PUBLIC TBB::tbb)
# target_compile_options(SRendererCore PRIVATE ${TBB_CXX_STD_FLAG})

target_compile_definitions(SRendererCore PRIVATE SRENDERERCORE_LIBRARY)
