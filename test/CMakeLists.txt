add_executable(migemo_test migemo.c)

target_link_libraries(migemo_test PRIVATE migemo_static)

target_include_directories(migemo_test PRIVATE ${PROJECT_SOURCE_DIR}/src)

target_compile_definitions(
  migemo_test
  PRIVATE
    TEST_DICTDIR_MIGEMO="${CMAKE_CURRENT_SOURCE_DIR}/_testdata"
    TEST_DICTDIR_ROMA2HIRA="${PROJECT_SOURCE_DIR}/dict")

add_test(NAME test_migemo COMMAND migemo_test)

#------------------------------------------------------------------------------

add_executable(romaji_test romaji.c)

target_link_libraries(romaji_test PRIVATE migemo_static)

target_include_directories(romaji_test PRIVATE ${PROJECT_SOURCE_DIR}/src)

target_compile_definitions(
  romaji_test
  PRIVATE
    TEST_DICTDIR_ROMA2HIRA="${PROJECT_SOURCE_DIR}/dict")

add_test(NAME test_romaji COMMAND romaji_test)

#------------------------------------------------------------------------------

add_executable(rxgen_test rxgen.c)

target_link_libraries(rxgen_test PRIVATE migemo_static)

target_include_directories(rxgen_test PRIVATE ${PROJECT_SOURCE_DIR}/src)

add_test(NAME test_rxgen COMMAND rxgen_test)

#------------------------------------------------------------------------------

add_custom_target(
  check
  COMMAND ${CMAKE_CTEST_COMMAND} --output-on-failure
  DEPENDS migemo_test romaji_test rxgen_test)
