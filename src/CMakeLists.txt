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
