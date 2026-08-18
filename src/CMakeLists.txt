configure_file(
  "${PROJECT_SOURCE_DIR}/include/migemo.h.in"
  "${CMAKE_CURRENT_BINARY_DIR}/include/migemo.h"
  @ONLY)

configure_file(
  "${CMAKE_CURRENT_SOURCE_DIR}/migemo.rc.in"
  "${CMAKE_CURRENT_BINARY_DIR}/migemo.rc"
  @ONLY)

#------------------------------------------------------------------------------

add_library(
  migemo_objects
  charset.c
  filename.c
  migemo.c
  mtree.c
  romaji.c
  rxgen.c
  trie.c
  strbuf.c
  wordlist.c)

target_include_directories(
  migemo_objects
  PUBLIC
    $<BUILD_INTERFACE:${PROJECT_SOURCE_DIR}/include>
    $<BUILD_INTERFACE:${CMAKE_CURRENT_BINARY_DIR}/include>)

set_target_properties(migemo_objects PROPERTIES POSITION_INDEPENDENT_CODE ON)

#------------------------------------------------------------------------------

add_library(
  migemo
  SHARED
    migemo.def "${CMAKE_CURRENT_BINARY_DIR}/migemo.rc"
    $<TARGET_OBJECTS:migemo_objects>)

target_include_directories(
  migemo
  PUBLIC
    $<BUILD_INTERFACE:${PROJECT_SOURCE_DIR}/include>
    $<BUILD_INTERFACE:${CMAKE_CURRENT_BINARY_DIR}/include>
    $<INSTALL_INTERFACE:${CMAKE_INSTALL_INCLUDEDIR}>)

set_target_properties(
  migemo
  PROPERTIES
    OUTPUT_NAME "migemo" VERSION ${PROJECT_VERSION} SOVERSION
    ${PROJECT_VERSION_MAJOR})

#------------------------------------------------------------------------------

add_library(migemo_static STATIC $<TARGET_OBJECTS:migemo_objects>)

target_include_directories(
  migemo_static
  PUBLIC
    ${PROJECT_SOURCE_DIR}/src $<BUILD_INTERFACE:${PROJECT_SOURCE_DIR}/include>
    $<BUILD_INTERFACE:${CMAKE_CURRENT_BINARY_DIR}/include>)

#------------------------------------------------------------------------------

install(
  TARGETS migemo
  RUNTIME DESTINATION ${CMAKE_INSTALL_BINDIR} COMPONENT core
  LIBRARY DESTINATION ${CMAKE_INSTALL_LIBDIR} COMPONENT core
  ARCHIVE DESTINATION ${CMAKE_INSTALL_LIBDIR} COMPONENT core)

install(
  FILES ${CMAKE_CURRENT_BINARY_DIR}/include/migemo.h
  DESTINATION ${CMAKE_INSTALL_INCLUDEDIR}
  COMPONENT core)
