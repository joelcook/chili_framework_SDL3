cmake_minimum_required(VERSION 3.20)
project(ChiliFramework LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

find_package(SDL3 REQUIRED)

set(ENGINE_SOURCES
    Engine/Game.cpp
    Engine/Game.h
    Engine/Keyboard.cpp
    Engine/Keyboard.h
    Engine/Mouse.cpp
    Engine/Mouse.h
    Engine/Colors.h
    Engine/ChiliException.h
    Engine/SoundEffect.h
)

if(APPLE)
    list(APPEND ENGINE_SOURCES
        Engine/MainWindow.cpp
        Engine/MainWindow.h
        Engine/Graphics.cpp
        Engine/Graphics.h
        Engine/Sound.cpp
        Engine/Sound.h
        Engine/Main.cpp
        Engine/ChiliSDL.h
    )
elseif(WIN32)
    list(APPEND ENGINE_SOURCES
        Engine/MainWindow.cpp
        Engine/MainWindow.h
        Engine/Graphics.cpp
        Engine/Graphics.h
        Engine/Sound.cpp
        Engine/Sound.h
        Engine/Main.cpp
        Engine/ChiliWin.h
        Engine/DXErr.cpp
        Engine/DXErr.h
        Engine/COMInitializer.cpp
        Engine/COMInitializer.h
        Engine/Resource.h
    )
endif()

add_executable(ChiliFramework ${ENGINE_SOURCES})

target_include_directories(ChiliFramework PRIVATE ${CMAKE_SOURCE_DIR}/Engine)

if(APPLE)
    target_link_libraries(ChiliFramework PRIVATE SDL3::SDL3)
    target_compile_definitions(ChiliFramework PRIVATE CHILI_USE_SDL)
elseif(WIN32)
    target_link_libraries(ChiliFramework PRIVATE d3d11.lib)
endif()
