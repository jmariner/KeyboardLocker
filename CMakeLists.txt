cmake_minimum_required(VERSION 3.20)
project(KeyboardLocker)

# Find dependencies provided by vcpkg (via vcpkg.cmake)
find_package(czmq CONFIG REQUIRED)

# set up Keyboardlocker.dll target
add_library(KeyboardLocker SHARED)
set_target_properties(KeyboardLocker PROPERTIES PREFIX "" SUFFIX ".dll")
# NOTE: need to use RUNTIME_OUTPUT_DIRECTORY instead of LIBRARY_OUTPUT_DIRECTORY for windows
set_target_properties(KeyboardLocker PROPERTIES RUNTIME_OUTPUT_DIRECTORY "${CMAKE_CURRENT_SOURCE_DIR}/lib")
target_sources(KeyboardLocker PRIVATE lib/KeyboardLocker.c)
target_link_libraries(KeyboardLocker PRIVATE czmq)

# custom target to call ahk2exe using the ahk script and the bin file to create the exe
find_program(AHK2EXE Ahk2Exe.exe PATHS "C:/Program Files/AutoHotkey/Compiler" "C:/Program Files (x86)/AutoHotkey/Compiler" REQUIRED)
set(AHK_SRC "${CMAKE_CURRENT_SOURCE_DIR}/lib/KeyboardLocker.ahk")
set(AHK_BIN "${CMAKE_CURRENT_SOURCE_DIR}/lib/KeyboardLocker.bin")
add_custom_target(
    KeyboardLockerExe ALL
    DEPENDS KeyboardLocker "${AHK_SRC}" "${AHK_BIN}"
    COMMAND "${AHK2EXE}" /in "${AHK_SRC}" /out "${CMAKE_CURRENT_BINARY_DIR}/KeyboardLocker.exe" /bin "${AHK_BIN}"
)