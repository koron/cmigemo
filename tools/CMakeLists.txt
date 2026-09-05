if(WIN32)
  set(CMIGEMO_DEFAULT_DICTDIR "${CMAKE_INSTALL_FULL_DATADIR}/cmigemo/cp932")
  set(DEFAULT_DICTDIR "${PROJECT_BINARY_DIR}/dict/mdict/cp932")
else()
  set(CMIGEMO_DEFAULT_DICTDIR "${CMAKE_INSTALL_FULL_DATADIR}/cmigemo/utf-8")
  set(DEFAULT_DICTDIR "${PROJECT_BINARY_DIR}/dict/mdict/utf-8")
endif()

#------------------------------------------------------------------------------

add_executable(cmigemo cmigemo.c)

target_link_libraries(cmigemo PRIVATE migemo)

target_compile_definitions(
  cmigemo
  PRIVATE CMIGEMO_DICTDIR="${CMIGEMO_DEFAULT_DICTDIR}")

install(
  TARGETS cmigemo
  RUNTIME DESTINATION ${CMAKE_INSTALL_BINDIR} COMPONENT core
  LIBRARY DESTINATION ${CMAKE_INSTALL_LIBDIR} COMPONENT core
  ARCHIVE DESTINATION ${CMAKE_INSTALL_LIBDIR} COMPONENT core)

#------------------------------------------------------------------------------

add_executable(romaji romaji.c)

target_compile_definitions(romaji PUBLIC DICTDIR="${DEFAULT_DICTDIR}")

target_link_libraries(romaji PRIVATE migemo_static)

#------------------------------------------------------------------------------

add_executable(gen_sdict gen_sdict.c)
target_link_libraries(gen_sdict PRIVATE migemo_static)
