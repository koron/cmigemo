add_executable(test1 main.c test1.c)
target_link_libraries(test1 PRIVATE migemo)
target_include_directories(test1 PRIVATE ${CMAKE_SOURCE_DIR}/src)

#------------------------------------------------------------------------------

add_test(NAME test_migemo1 COMMAND test1)

# Configuration for loading test dictionaries
set_tests_properties(
  test_migemo1
  PROPERTIES WORKING_DIRECTORY ${CMAKE_SOURCE_DIR}/test)

#------------------------------------------------------------------------------

add_custom_target(
  check
  COMMAND ${CMAKE_CTEST_COMMAND} --output-on-failure
  DEPENDS test1)
