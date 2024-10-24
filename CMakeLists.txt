cmake_minimum_required(VERSION 3.16)
include(GNUInstallDirs)

set(PROJ KanoopGit)
project(${PROJ})

find_package(Qt6 REQUIRED COMPONENTS Network Xml Gui Widgets)
qt_standard_project_setup()

set(CMAKE_CXX_STANDARD 11)
set(CMAKE_C_STANDARD_REQUIRED ON)
set(KANOOPGIT_VERSION "0.0.10")

set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTORCC ON)
set(CMAKE_AUTOUIC ON)

file(GLOB_RECURSE KANOOPGIT_SOURCES
    "${CMAKE_CURRENT_SOURCE_DIR}/*.cpp"
    "${CMAKE_CURRENT_SOURCE_DIR}/*.h"
    "${CMAKE_CURRENT_SOURCE_DIR}/*.ui"
    "${CMAKE_CURRENT_SOURCE_DIR}/*.qrc"
)

qt_add_executable(${PROJ} ${KANOOPGIT_SOURCES} ${KANOOPGIT_HEADERS})

target_compile_options(${PROJ} PRIVATE -Wextra -Wall -Werror)
add_compile_definitions(KANOOPGIT_VERSION=${KANOOPGIT_VERSION})

set_target_properties(${PROJ} PROPERTIES VERSION "${KANOOPGIT_VERSION}")
set_target_properties(${PROJ} PROPERTIES SOVERSION 1)

target_include_directories(${PROJ} PUBLIC ${CMAKE_CURRENT_SOURCE_DIR}/include)
target_include_directories(${PROJ} PRIVATE ${CMAKE_CURRENT_SOURCE_DIR}/include/Kanoop)
target_include_directories(${PROJ} PRIVATE ${CMAKE_CURRENT_SOURCE_DIR}/src)

get_directory_property(PARENT_DIR PARENT_DIRECTORY)
target_include_directories(${PROJ} PRIVATE ${PARENT_DIR}/libgit2/include)
target_link_directories(${PROJ} PRIVATE ${CMAKE_BINARY_DIR}/libgit2)

target_link_libraries(${PROJ} PRIVATE Qt6::Network Qt6::Xml Qt6::Gui Qt6::Widgets KanoopCommon KanoopQtGui git2qt)

add_compile_definitions(QT_DEPRECATED_WARNINGS)
add_compile_definitions(QT_DISABLE_DEPRECATED_BEFORE=0x060000)  # Disables all the APIs deprecated before Qt 6.0.0

install(TARGETS ${PROJ} LIBRARY DESTINATION "${KANOOP_INSTALL_PREFIX}${CMAKE_INSTALL_PREFIX}/${CMAKE_INSTALL_LIBDIR}")
