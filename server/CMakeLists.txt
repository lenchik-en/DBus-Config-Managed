set (SOURCE main.cpp
        ConfigurationObject.cpp)

set (HEADER ConfigurationObject.h)

add_executable(server ${SOURCE} ${HEADER})
target_link_libraries(server Qt5::Core Qt5::DBus)
target_include_directories(server PRIVATE ${CMAKE_CURRENT_SOURCE_DIR})