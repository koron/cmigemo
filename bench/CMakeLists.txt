#------------------------------------------------------------------------------

get_target_property(MIGEMO_SOURCES migemo_objects SOURCES)
get_target_property(MIGEMO_DIR migemo_objects SOURCE_DIR)
list(TRANSFORM MIGEMO_SOURCES PREPEND "${MIGEMO_DIR}/" REGEX "^[^/]")

add_library(migemo_profile STATIC ${MIGEMO_SOURCES})

target_include_directories(
  migemo_profile
  PUBLIC ${PROJECT_SOURCE_DIR}/src ${PROJECT_BINARY_DIR}/src/include)

target_compile_definitions(
  migemo_profile
  PUBLIC DICTDIR="${PROJECT_BINARY_DIR}/dict/utf-8")

if(BUILD_DICT)
  add_dependencies(migemo_profile dict_utf-8)
endif()

find_program(GPROF_EXECUTABLE gprof)

if(GPROF_EXECUTABLE AND CMAKE_C_COMPILER_ID MATCHES "GNU|Clang")
  target_compile_options(migemo_profile PUBLIC -pg -fno-omit-frame-pointer)
  target_link_options(migemo_profile INTERFACE -pg -fno-omit-frame-pointer)
endif()

#------------------------------------------------------------------------------
# profile_char_query

add_executable(profile_char_query profile_char_query.c)
target_link_libraries(profile_char_query PRIVATE migemo_profile)

add_executable(profile_migemo_open profile_migemo_open.c)
target_link_libraries(profile_migemo_open PRIVATE migemo_profile)

#------------------------------------------------------------------------------

add_custom_target(
  profile
  COMMAND $<TARGET_FILE:profile_char_query>
  COMMAND ${CMAKE_COMMAND} -E echo "Generating profile.log..."
  COMMAND ${CMAKE_COMMAND} -P "${CMAKE_CURRENT_SOURCE_DIR}/copy_gmon.cmake"
  COMMAND
    ${GPROF_EXECUTABLE} $<TARGET_FILE:profile_char_query> gmon.out > profile.log
  WORKING_DIRECTORY ${PROJECT_BINARY_DIR}
  DEPENDS profile_char_query
  COMMENT "Running profile_char_query benchmark and generating profile.log")
