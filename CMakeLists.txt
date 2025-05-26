cmake_minimum_required(VERSION 3.16)
project(nmconeditor LANGUAGES CXX)

set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTORCC ON)
set(CMAKE_AUTOUIC ON)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

find_package(Qt5 COMPONENTS Core Gui Qml Network Widgets REQUIRED) # Widgets for some NMQt dependencies if any

find_package(ECM 5.71 NO_MODULE)
set(CMAKE_MODULE_PATH ${ECM_MODULE_PATH} ${CMAKE_CURRENT_SOURCE_DIR}/cmake)
include(FeatureSummary)

find_package(KF5 ${KF5_MIN_VERSION} REQUIRED COMPONENTS
    # CoreAddons      # KAboutData
    NetworkManagerQt
)

# find_package(KF5 COMPONENTS CoreAddons I18n NetworkManagerQt REQUIRED) # Use NetworkManagerQt5 for Qt5

# Add other Qt modules if your QML uses them, e.g., QtQuick.Controls

set(PROJECT_SOURCES
    main.cpp
)

# Create an executable
add_executable(${PROJECT_NAME} ${PROJECT_SOURCES}
    main.cpp
    addcon.cpp
)

# Link Qt libraries
target_link_libraries(${PROJECT_NAME} PRIVATE
    Qt5::Core
    Qt5::Gui
    Qt5::Qml
    Qt5::Network
    Qt5::Widgets
    KF5::NetworkManagerQt # Or NetworkManagerQt5::NetworkManagerQt
)

# Define the QML import path if your QML files are in a subdirectory
# target_include_directories(${PROJECT_NAME} PRIVATE
#     ${CMAKE_CURRENT_SOURCE_DIR}/qml
# )

install(TARGETS ${PROJECT_NAME}
    RUNTIME DESTINATION bin
)
