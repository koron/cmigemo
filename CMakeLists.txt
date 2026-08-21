cmake_minimum_required(VERSION 3.16)

project(
  cmigemo
  VERSION 1.7.0
  DESCRIPTION "C/Migemo library and tool"
  LANGUAGES C)

set(PROJECT_VERSION_PRERELEASE "")

set(PROJECT_VERSION_FULL "${PROJECT_VERSION}${PROJECT_VERSION_PRERELEASE}")

set(CMAKE_C_STANDARD 11)
set(CMAKE_C_STANDARD_REQUIRED ON)

#------------------------------------------------------------------------------

if(NOT CMAKE_BUILD_TYPE)
  set(CMAKE_BUILD_TYPE Release CACHE STRING "Choose the type of build." FORCE)
endif()

option(BUILD_DICT "Build dictionary files" ON)
option(BUILD_TESTING "Build tests" ON)

#------------------------------------------------------------------------------

include(GNUInstallDirs)
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${PROJECT_BINARY_DIR}/${CMAKE_INSTALL_BINDIR})
set(CMAKE_LIBRARY_OUTPUT_DIRECTORY ${PROJECT_BINARY_DIR}/${CMAKE_INSTALL_LIBDIR})
set(CMAKE_ARCHIVE_OUTPUT_DIRECTORY ${PROJECT_BINARY_DIR}/${CMAKE_INSTALL_LIBDIR})

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

# Disable MSVC warnings
if(MSVC)
  add_compile_options(/utf-8 /W3 /wd4710 /wd4711 /wd5045)
  add_compile_definitions(_CRT_SECURE_NO_WARNINGS)
endif()

#------------------------------------------------------------------------------

add_subdirectory(src)
add_subdirectory(tools)
if(BUILD_DICT)
  add_subdirectory(dict)
endif()
if(BUILD_TESTING)
  enable_testing()
  add_subdirectory(test)
  add_subdirectory(bench)
endif()
if(EMSCRIPTEN)
  add_subdirectory(wasm)
endif()

#------------------------------------------------------------------------------

set(CPACK_PACKAGE_VENDOR "koron")

if(WIN32)
  set(CPACK_GENERATOR "ZIP")
else()
  set(CPACK_GENERATOR "TGZ")
endif()

if(WIN32)
  if(MSVC)
    set(SYSTEM_SUFFIX "-msvc")
  elseif(DEFINED ENV{MSYSTEM} AND NOT "$ENV{MSYSTEM}" STREQUAL "")
    set(SYSTEM_SUFFIX "-$ENV{MSYSTEM}")
  endif()
endif()

string(TOLOWER "${CMAKE_SYSTEM_NAME}${SYSTEM_SUFFIX}-${CMAKE_SYSTEM_PROCESSOR}"
       SYSTEM_LOWER)
set(CPACK_PACKAGE_FILE_NAME
    "${CMAKE_PROJECT_NAME}-${PROJECT_VERSION_FULL}-${SYSTEM_LOWER}")

include(CPack)
