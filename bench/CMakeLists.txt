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

add_executable(bench_char_query char_query.c)
target_link_libraries(bench_char_query PRIVATE migemo_profile)

add_executable(bench_migemo_open migemo_open.c)
target_link_libraries(bench_migemo_open PRIVATE migemo_profile)

#------------------------------------------------------------------------------

function(add_profile_target target_name)
    add_custom_target(profile_${target_name}
        COMMAND $<TARGET_FILE:bench_${target_name}>
        COMMAND ${GPROF_EXECUTABLE} $<TARGET_FILE:bench_${target_name}> gmon.out > profile_${target_name}.log
        COMMAND ${CMAKE_COMMAND} -E remove gmon.out
        WORKING_DIRECTORY ${PROJECT_BINARY_DIR}
        DEPENDS bench_${target_name}
        COMMENT "Profiling ${target_name} -> profile_${target_name}.log"
    )
endfunction()

add_profile_target(char_query)
add_profile_target(migemo_open)

add_custom_target(profile
    DEPENDS profile_char_query profile_migemo_open
    COMMENT "Running all profiles"
)
