get_target_property(MIGEMO_SOURCES migemo_objects SOURCES)
get_target_property(MIGEMO_DIR migemo_objects SOURCE_DIR)
list(TRANSFORM MIGEMO_SOURCES PREPEND "${MIGEMO_DIR}/" REGEX "^[^/]")

#------------------------------------------------------------------------------

add_executable(migemo_wasm ${MIGEMO_SOURCES})

target_include_directories(
  migemo_wasm
  PRIVATE ${CMAKE_SOURCE_DIR}/include ${PROJECT_BINARY_DIR}/src/include)

set_target_properties(
  migemo_wasm
  PROPERTIES OUTPUT_NAME "migemo_wasm" SUFFIX ".js")

target_link_options(
  migemo_wasm
  PRIVATE
    "--js-library"
    "${CMAKE_CURRENT_SOURCE_DIR}/library.js"
    "-sEXPORTED_FUNCTIONS=['_migemo_open','_migemo_open_sdict','_migemo_switch_sdict','_migemo_save_sdict','_migemo_close','_migemo_query','_migemo_release','_migemo_load','_migemo_is_enable','_migemo_set_escape_chars','_migemo_version','_malloc','_free']"
    "-sEXPORTED_RUNTIME_METHODS=['FS','ccall','cwrap','UTF8ToString','allocate','ALLOC_NORMAL']"
    "-sMODULARIZE=1"
    "-sEXPORT_NAME='createMigemoModule'"
    "-sEXPORT_ES6=1"
    "-sENVIRONMENT=web,node"
    "-sALLOW_MEMORY_GROWTH=1")

add_custom_command(
  TARGET migemo_wasm
  POST_BUILD
  COMMAND
    ${CMAKE_COMMAND} -E copy $<TARGET_FILE_DIR:migemo_wasm>/migemo_wasm.js
    ${CMAKE_CURRENT_SOURCE_DIR}/migemo_wasm.js
  COMMAND
    ${CMAKE_COMMAND} -E copy $<TARGET_FILE_DIR:migemo_wasm>/migemo_wasm.wasm
    ${CMAKE_CURRENT_SOURCE_DIR}/migemo_wasm.wasm)
