cmake_minimum_required(VERSION 3.16)
project(AircraftRocketTracker VERSION 1.0 LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# Find Qt6 or Qt5
find_package(Qt6 COMPONENTS Core Widgets Network Positioning QUIET)
if(NOT Qt6_FOUND)
    find_package(Qt5 5.15 REQUIRED COMPONENTS Core Widgets Network Positioning)
    set(QT_VERSION_MAJOR 5)
else()
    set(QT_VERSION_MAJOR 6)
endif()

set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTORCC ON)
set(CMAKE_AUTOUIC ON)

set(PROJECT_SOURCES
    src/main.cpp
    src/mainwindow.cpp
    src/mainwindow.h
    src/aircraft.h
    src/aircraft.cpp
    src/rocket.h
    src/rocket.cpp
    src/geoutils.h
    src/geoutils.cpp
    src/mapwidget.h
    src/mapwidget.cpp
    src/adsbclient.h
    src/adsbclient.cpp
    src/rocketclient.h
    src/rocketclient.cpp
    src/systemgps.h
    src/systemgps.cpp
    src/gpsnavigator.h
    src/gpsnavigator.cpp
)

if(${QT_VERSION_MAJOR} GREATER_EQUAL 6)
    add_executable(${PROJECT_NAME} ${PROJECT_SOURCES})
    target_link_libraries(${PROJECT_NAME} PRIVATE 
        Qt6::Core 
        Qt6::Widgets 
        Qt6::Network
        Qt6::Positioning
    )
else()
    add_executable(${PROJECT_NAME} ${PROJECT_SOURCES})
    target_link_libraries(${PROJECT_NAME} PRIVATE 
        Qt5::Core 
        Qt5::Widgets 
        Qt5::Network
        Qt5::Positioning
    )
endif()

# Install target
install(TARGETS ${PROJECT_NAME}
    RUNTIME DESTINATION bin
)
