cmake_minimum_required(VERSION 3.25)
project(DBus_Config_Managed)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_AUTOMOC ON)
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

find_package(Qt5 REQUIRED COMPONENTS Core DBus)

add_subdirectory(server)