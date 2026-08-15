add_executable(test1 main.c test1.c)

target_link_libraries(test1 PRIVATE migemo)

target_include_directories(test1 PRIVATE ${PROJECT_SOURCE_DIR}/src)

target_compile_definitions(
  test1
  PRIVATE
    TEST_DICTDIR_MIGEMO="${CMAKE_CURRENT_SOURCE_DIR}/test1"
    TEST_DICTDIR_ROMA2HIRA="${PROJECT_SOURCE_DIR}/dict")

#------------------------------------------------------------------------------

add_test(NAME test_migemo1 COMMAND test1)

# Configuration for loading test dictionaries
set_tests_properties(
  test_migemo1
  PROPERTIES WORKING_DIRECTORY ${PROJECT_SOURCE_DIR}/test)

#------------------------------------------------------------------------------

add_custom_target(
  check
  COMMAND ${CMAKE_CTEST_COMMAND} --output-on-failure
  DEPENDS test1)
