if(WIN32)
  set(CMIGEMO_DEFAULT_DICTDIR "${CMAKE_INSTALL_FULL_DATADIR}/cmigemo/cp932")
else()
  set(CMIGEMO_DEFAULT_DICTDIR "${CMAKE_INSTALL_FULL_DATADIR}/cmigemo/utf-8")
endif()

configure_file(
  "${PROJECT_SOURCE_DIR}/include/migemo.h.in"
  "${CMAKE_CURRENT_BINARY_DIR}/include/migemo.h"
  @ONLY)

configure_file(
  "${CMAKE_CURRENT_SOURCE_DIR}/migemo.rc.in"
  "${CMAKE_CURRENT_BINARY_DIR}/migemo.rc"
  @ONLY)

add_library(
  migemo
  SHARED
    migemo.def
    "${CMAKE_CURRENT_BINARY_DIR}/migemo.rc"
    charset.c
    filename.c
    migemo.c
    mtree.c
    romaji.c
    rxgen.c
    trie.c
    strbuf.c
    wordlist.c)

set_target_properties(
  migemo
  PROPERTIES
    OUTPUT_NAME "migemo" VERSION ${PROJECT_VERSION} SOVERSION
    ${PROJECT_VERSION_MAJOR})

target_include_directories(
  migemo
  PUBLIC
    $<BUILD_INTERFACE:${PROJECT_SOURCE_DIR}/include>
    $<BUILD_INTERFACE:${CMAKE_CURRENT_BINARY_DIR}/include>
    $<INSTALL_INTERFACE:${CMAKE_INSTALL_INCLUDEDIR}>)

add_executable(cmigemo main.c)

target_link_libraries(cmigemo PRIVATE migemo)

target_compile_definitions(
  cmigemo
  PRIVATE CMIGEMO_DICTDIR="${CMIGEMO_DEFAULT_DICTDIR}")

#------------------------------------------------------------------------------

install(
  TARGETS cmigemo migemo
  RUNTIME DESTINATION ${CMAKE_INSTALL_BINDIR} COMPONENT core
  LIBRARY DESTINATION ${CMAKE_INSTALL_LIBDIR} COMPONENT core
  ARCHIVE DESTINATION ${CMAKE_INSTALL_LIBDIR} COMPONENT core)

install(
  FILES ${CMAKE_CURRENT_BINARY_DIR}/include/migemo.h
  DESTINATION ${CMAKE_INSTALL_INCLUDEDIR}
  COMPONENT core)
