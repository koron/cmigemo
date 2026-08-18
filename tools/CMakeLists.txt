if(WIN32)
  set(CMIGEMO_DEFAULT_DICTDIR "${CMAKE_INSTALL_FULL_DATADIR}/cmigemo/cp932")
else()
  set(CMIGEMO_DEFAULT_DICTDIR "${CMAKE_INSTALL_FULL_DATADIR}/cmigemo/utf-8")
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
