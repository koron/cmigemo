if (WIN32)
  set(CMIGEMO_DEFAULT_DICTDIR "${CMAKE_INSTALL_FULL_DATADIR}/cmigemo/cp932")
else()
  set(CMIGEMO_DEFAULT_DICTDIR "${CMAKE_INSTALL_FULL_DATADIR}/cmigemo/utf-8")
endif()

add_library(
  migemo
  SHARED
    migemo.def
    charset.c
    filename.c
    migemo.c
    mnode.c
    romaji.c
    rxgen.c
    wordbuf.c
    wordlist.c)

set_target_properties(
  migemo
  PROPERTIES OUTPUT_NAME "migemo" VERSION 1.4.0 SOVERSION 1)

add_executable(cmigemo main.c)
target_link_libraries(cmigemo PRIVATE migemo)
target_compile_definitions(cmigemo PRIVATE
  CMIGEMO_DICTDIR="${CMIGEMO_DEFAULT_DICTDIR}")

#------------------------------------------------------------------------------

install(
  TARGETS cmigemo migemo
  RUNTIME DESTINATION ${CMAKE_INSTALL_BINDIR}
  LIBRARY DESTINATION ${CMAKE_INSTALL_LIBDIR}
  ARCHIVE DESTINATION ${CMAKE_INSTALL_LIBDIR})

install(FILES migemo.h DESTINATION ${CMAKE_INSTALL_INCLUDEDIR})
