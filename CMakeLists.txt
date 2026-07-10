cmake_minimum_required(VERSION 3.16)

project(QtHeatmapViewer LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_AUTOMOC ON)

find_package(Qt6 COMPONENTS Widgets REQUIRED)


add_executable(QtHeatmapViewer WIN32
    main.cpp
    MainWindow.h
    MainWindow.cpp
    HeatmapWidget.h
    HeatmapWidget.cpp
    HeatmapData.h
    HeatmapData.cpp
)

target_link_libraries(QtHeatmapViewer PRIVATE Qt6::Widgets)

# For Qt5, replace the find_package/target_link_libraries lines with:
# find_package(Qt5 COMPONENTS Widgets REQUIRED)
# target_link_libraries(QtHeatmapViewer PRIVATE Qt5::Widgets)
