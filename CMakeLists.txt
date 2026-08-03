cmake_minimum_required(VERSION 3.16)

project(cmigemo
    VERSION 1.4.0
    DESCRIPTION "C/Migemo library and tool"
    LANGUAGES C
)

set(CMAKE_C_STANDARD 99)
set(CMAKE_C_STANDARD_REQUIRED ON)

#------------------------------------------------------------------------------

if(NOT CMAKE_BUILD_TYPE)
    set(CMAKE_BUILD_TYPE Release CACHE STRING "Choose the type of build." FORCE)
endif()

option(BUILD_TESTING "Build tests" ON)

#------------------------------------------------------------------------------

include(GNUInstallDirs)
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/${CMAKE_INSTALL_BINDIR})
set(CMAKE_LIBRARY_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/${CMAKE_INSTALL_LIBDIR})
set(CMAKE_ARCHIVE_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/${CMAKE_INSTALL_LIBDIR})

#------------------------------------------------------------------------------

include(CheckCCompilerFlag)

# -Wall
check_c_compiler_flag("-Wall" COMPILER_SUPPORTS_WALL)
if(COMPILER_SUPPORTS_WALL)
    add_compile_options(-Wall)
endif()

# -Wno-pointer-sign
check_c_compiler_flag("-Wno-pointer-sign" COMPILER_SUPPORTS_WNO_POINTER_SIGN)
if(COMPILER_SUPPORTS_WNO_POINTER_SIGN)
    add_compile_options(-Wno-pointer-sign)
endif()

# Optimization flags
if(CMAKE_C_COMPILER_ID MATCHES "GNU|Clang")
    add_compile_options($<$<CONFIG:Release>:-O2>)
endif()

if(MSVC)
    add_compile_options(/utf-8 /W3 /wd5045)
    add_compile_definitions(_CRT_SECURE_NO_WARNINGS)
endif()

#------------------------------------------------------------------------------

add_subdirectory(src)
add_subdirectory(dict)
if(BUILD_TESTING)
    enable_testing()
    add_subdirectory(test)
    add_subdirectory(src/testdir)
endif()
