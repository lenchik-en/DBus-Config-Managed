add_executable(confManagerApplication1 main.cpp)
target_link_libraries(confManagerApplication1 Qt5::Core Qt5::DBus)

target_include_directories(confManagerApplication1 PRIVATE ${CMAKE_CURRENT_SOURCE_DIR})