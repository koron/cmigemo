set(ALL_TEST_TARGETS "")

function(add_migemo_test name src)
  set(target "${name}_test")

  add_executable(${target} ${src})

  target_link_libraries(${target} PRIVATE migemo_static)

  target_include_directories(${target} PRIVATE ${PROJECT_SOURCE_DIR}/src)

  if(ARGN)
    target_compile_definitions(${target} PRIVATE ${ARGN})
  endif()

  add_test(NAME "test_${name}" COMMAND ${target})

  set(ALL_TEST_TARGETS ${ALL_TEST_TARGETS} ${target} PARENT_SCOPE)
endfunction()

#------------------------------------------------------------------------------

add_migemo_test(
  migemo migemo.c TEST_DICTDIR_MIGEMO="${CMAKE_CURRENT_SOURCE_DIR}/_testdata"
  TEST_DICTDIR_ROMA2HIRA="${PROJECT_SOURCE_DIR}/dict")

add_migemo_test(romaji romaji.c
                TEST_DICTDIR_ROMA2HIRA="${PROJECT_SOURCE_DIR}/dict")

add_migemo_test(rxgen rxgen.c)

#------------------------------------------------------------------------------

add_custom_target(
  check
  COMMAND ${CMAKE_CTEST_COMMAND} --output-on-failure
  DEPENDS ${ALL_TEST_TARGETS})
