cmake_minimum_required(VERSION 3.20)
project(KeyboardLocker)
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

# Find dependencies provided by vcpkg (via vcpkg.cmake)
find_package(czmq CONFIG REQUIRED)

# set up Keyboardlocker.exe target
add_executable(KeyboardLocker)
target_sources(KeyboardLocker PRIVATE src/KeyboardLocker.cpp res/KeyboardLocker.rc)
add_subdirectory(${CMAKE_CURRENT_SOURCE_DIR}/vendor/traypp ${CMAKE_CURRENT_BINARY_DIR}/traypp)
target_link_libraries(KeyboardLocker PRIVATE czmq tray winmm.lib)

target_compile_features(KeyboardLocker PUBLIC cxx_std_17)
set_target_properties(KeyboardLocker PROPERTIES CMAKE_CXX_STANDARD 17)
set_target_properties(KeyboardLocker PROPERTIES CMAKE_CXX_EXTENSIONS Off)
set_target_properties(KeyboardLocker PROPERTIES CMAKE_CXX_STANDARD_REQUIRED On)