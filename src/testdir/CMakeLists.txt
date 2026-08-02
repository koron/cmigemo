#------------------------------------------------------------------------------
# romaji tool (use migemo shared object)
add_executable(romaji romaji_main.c)
target_link_libraries(romaji PRIVATE migemo)
target_include_directories(romaji PRIVATE ${CMAKE_SOURCE_DIR}/src)
target_compile_definitions(romaji PRIVATE
	DICTDIR="${CMAKE_BINARY_DIR}/dict/utf-8.d"
    _DEBUG
)

#------------------------------------------------------------------------------
# qspeed: profiling tool
add_executable(qspeed
    profile_speed.c
    ${CMAKE_SOURCE_DIR}/src/charset.c
    ${CMAKE_SOURCE_DIR}/src/filename.c
    ${CMAKE_SOURCE_DIR}/src/migemo.c
    ${CMAKE_SOURCE_DIR}/src/mnode.c
    ${CMAKE_SOURCE_DIR}/src/romaji.c
    ${CMAKE_SOURCE_DIR}/src/rxgen.c
    ${CMAKE_SOURCE_DIR}/src/wordbuf.c
    ${CMAKE_SOURCE_DIR}/src/wordlist.c
)
target_include_directories(qspeed PRIVATE ${CMAKE_SOURCE_DIR}/src)
target_compile_definitions(qspeed PRIVATE
    DICTDIR="${CMAKE_BINARY_DIR}/dict/utf-8.d"
    _DEBUG
)

#------------------------------------------------------------------------------
# Profiling target "profile": `cmake --build build --target profile`
find_program(GPROF_EXECUTABLE gprof)

if(GPROF_EXECUTABLE AND CMAKE_C_COMPILER_ID MATCHES "GNU|Clang")
    target_compile_options(qspeed PRIVATE -pg)
    target_link_options(qspeed PRIVATE -pg)

    add_custom_target(profile
        COMMAND $<TARGET_FILE:qspeed>

        COMMAND ${CMAKE_COMMAND} -E copy_if_different qspeed.gmon gmon.out

        COMMAND ${CMAKE_COMMAND} -E echo "Generating profile.log..."
        COMMAND ${GPROF_EXECUTABLE} $<TARGET_FILE:qspeed> gmon.out > profile.log

	WORKING_DIRECTORY ${CMAKE_BINARY_DIR}
        DEPENDS qspeed
        COMMENT "Running qspeed benchmark and generating profile.log"
    )
endif()
