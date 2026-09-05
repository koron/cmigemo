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
  PUBLIC DICTDIR="${PROJECT_BINARY_DIR}/dict/mdict/utf-8")

if(BUILD_DICT)
  add_dependencies(migemo_profile mdict_utf-8)
endif()

find_program(GPROF_EXECUTABLE gprof)

if(GPROF_EXECUTABLE AND CMAKE_C_COMPILER_ID MATCHES "GNU|Clang")
  target_compile_options(migemo_profile PUBLIC -pg -fno-omit-frame-pointer)
  target_link_options(migemo_profile INTERFACE -pg -fno-omit-frame-pointer)
endif()

#------------------------------------------------------------------------------

function(add_profile_target target_name)
  set(bench_target "bench_${target_name}")
  set(profile_target "profile_${target_name}")

  add_executable(${bench_target} "${target_name}.c")
  target_link_libraries(${bench_target} PRIVATE migemo_profile)

  set(work_dir "${PROJECT_BINARY_DIR}/bench/${target_name}")
  file(MAKE_DIRECTORY "${work_dir}")

  add_custom_target(
    ${profile_target}
    COMMAND $<TARGET_FILE:${bench_target}>
    COMMAND
      ${CMAKE_COMMAND} -DSRC="$<TARGET_FILE_NAME:${bench_target}>.gmon"
      -DDST=gmon.out -P "${CMAKE_CURRENT_SOURCE_DIR}/copy_if_exists.cmake"
    COMMAND
      ${GPROF_EXECUTABLE} $<TARGET_FILE:${bench_target}> gmon.out >
      "${PROJECT_BINARY_DIR}/bench/${profile_target}.log"
    COMMAND
      ${CMAKE_COMMAND} -E remove gmon.out
      "$<TARGET_FILE_NAME:${bench_target}>.gmon"
    WORKING_DIRECTORY ${work_dir}
    DEPENDS ${bench_target}
    COMMENT "Profiling ${target_name} -> ${profile_target}.log")

  set_property(DIRECTORY APPEND PROPERTY ALL_PROFILE_TARGETS ${profile_target})
endfunction()

add_profile_target(char_query)
add_profile_target(migemo_open)
# Add it here when adding a benchmark target.

get_property(all_profiles DIRECTORY PROPERTY ALL_PROFILE_TARGETS)

add_custom_target(
  profile
  DEPENDS ${all_profiles}
  COMMENT "Running all profiles")
