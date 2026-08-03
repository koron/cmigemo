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

#------------------------------------------------------------------------------

install (
  TARGETS cmigemo migemo
  RUNTIME DESTINATION ${CMAKE_INSTALL_BINDIR}
  LIBRARY DESTINATION ${CMAKE_INSTALL_LIBDIR}
  ARCHIVE DESTINATION ${CMAKE_INSTALL_LIBDIR})

install (FILES migemo.h DESTINATION ${CMAKE_INSTALL_INCLUDEDIR})
