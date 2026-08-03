find_package(Perl REQUIRED)

#------------------------------------------------------------------------------
# Detect required commands: curl, iconv, gzip
find_program(CURL_EXECUTABLE curl REQUIRED)
find_program(ICONV_EXECUTABLE iconv REQUIRED)
find_program(GZIP_EXECUTABLE gzip REQUIRED)

#------------------------------------------------------------------------------
set(SKKDICT_BASEURL "https://skk-dev.github.io/dict")
set(SKKDICT_FILE "SKK-JISYO.L")
set(DICT_FILE "migemo-dict")
set(DAT_FILES han2zen.dat hira2kata.dat roma2hira.dat zen2han.dat)

set(SKKDICT_PATH "${CMAKE_CURRENT_BINARY_DIR}/${SKKDICT_FILE}")
set(BASE_DICT_PATH "${CMAKE_CURRENT_BINARY_DIR}/${DICT_FILE}")

#------------------------------------------------------------------------------
# Download the SKK dictionary and convert it to UTF-8
add_custom_command(
  OUTPUT ${SKKDICT_PATH}
  COMMAND
    ${CURL_EXECUTABLE} -fsSLO "${SKKDICT_BASEURL}/${SKKDICT_FILE}.gz" # Download
  COMMAND ${GZIP_EXECUTABLE} -d "${SKKDICT_FILE}.gz" # Extract
  COMMAND
    ${ICONV_EXECUTABLE}
    -f
    EUC-JISX0213
    -t
    UTF-8
    ${SKKDICT_FILE}
    >
    ${SKKDICT_FILE}.utf-8 # Conert EUC-JISX0213 -> UTF-8
  COMMAND
    ${CMAKE_COMMAND} -E rename ${SKKDICT_FILE}.utf-8 ${SKKDICT_FILE}
    # Remove temporary files
  COMMAND ${CMAKE_COMMAND} -E remove "${SKKDICT_FILE}.gz"
  WORKING_DIRECTORY ${CMAKE_CURRENT_BINARY_DIR}
  COMMENT "Downloading and converting ${SKKDICT_FILE} to UTF-8")

#------------------------------------------------------------------------------
# Generate C/Migemo dictionary file (migemo-dict).
add_custom_command(
  OUTPUT ${BASE_DICT_PATH}
  COMMAND
    ${PERL_EXECUTABLE} ${CMAKE_CURRENT_SOURCE_DIR}/skk2migemo.pl <
    ${SKKDICT_PATH} > ${CMAKE_CURRENT_BINARY_DIR}/tmp.dict
  COMMAND
    ${PERL_EXECUTABLE} ${CMAKE_CURRENT_SOURCE_DIR}/optimize-dict.pl <
    ${CMAKE_CURRENT_BINARY_DIR}/tmp.dict > ${BASE_DICT_PATH}
  COMMAND ${CMAKE_COMMAND} -E remove ${CMAKE_CURRENT_BINARY_DIR}/tmp.dict
  DEPENDS
    ${SKKDICT_PATH} ${CMAKE_CURRENT_SOURCE_DIR}/skk2migemo.pl
    ${CMAKE_CURRENT_SOURCE_DIR}/optimize-dict.pl
  WORKING_DIRECTORY ${CMAKE_CURRENT_BINARY_DIR}
  COMMENT "Generating base migemo-dict")

#------------------------------------------------------------------------------
# A helper function that generates a dictionary for the specified encoding.
function(add_dict_encoding_target ENCODING_NAME ICONV_ENCODING IGNORE_ERROR)
  set(OUT_DIR "${CMAKE_CURRENT_BINARY_DIR}/${ENCODING_NAME}.d")
  set(OUT_DICT "${OUT_DIR}/${DICT_FILE}")

  set(COMMANDS_LIST "")

  list(
    APPEND
    COMMANDS_LIST
    COMMAND
    ${CMAKE_COMMAND}
    -E
    make_directory
    ${OUT_DIR})

  foreach(dat ${DAT_FILES})
    if(ICONV_ENCODING STREQUAL "UTF-8")
      list(
        APPEND
        COMMANDS_LIST
        COMMAND
        ${CMAKE_COMMAND}
        -E
        copy
        ${CMAKE_CURRENT_SOURCE_DIR}/${dat}
        ${OUT_DIR}/${dat})
    else()
      list(
        APPEND
        COMMANDS_LIST
        COMMAND
        ${ICONV_EXECUTABLE}
        -f
        UTF-8
        -t
        ${ICONV_ENCODING}
        ${CMAKE_CURRENT_SOURCE_DIR}/${dat}
        >
        ${OUT_DIR}/${dat})
    endif()
  endforeach()

  # Copy migemo-dict-zh (Chinese dictionary)
  if(EXISTS "${CMAKE_CURRENT_SOURCE_DIR}/migemo-dict-zh")
    list(
      APPEND
      COMMANDS_LIST
      COMMAND
      ${CMAKE_COMMAND}
      -E
      copy
      ${CMAKE_CURRENT_SOURCE_DIR}/migemo-dict-zh
      ${OUT_DIR}/)
  endif()

  # Convert and copy the main dictionary: migemo-dict
  if(ICONV_ENCODING STREQUAL "UTF-8")
    list(
      APPEND
      COMMANDS_LIST
      COMMAND
      ${CMAKE_COMMAND}
      -E
      copy
      ${BASE_DICT_PATH}
      ${OUT_DICT})
  else()
    if(IGNORE_ERROR)
      list(
        APPEND
        COMMANDS_LIST
        COMMAND
        ${ICONV_EXECUTABLE}
        -cs
        -f
        UTF-8
        -t
        ${ICONV_ENCODING}
        ${BASE_DICT_PATH}
        >
        ${OUT_DICT}
        ||
        ${CMAKE_COMMAND}
        -E
        true)
    else()
      list(
        APPEND
        COMMANDS_LIST
        COMMAND
        ${ICONV_EXECUTABLE}
        -f
        UTF-8
        -t
        ${ICONV_ENCODING}
        ${BASE_DICT_PATH}
        >
        ${OUT_DICT})
    endif()
  endif()

  add_custom_command(
    OUTPUT ${OUT_DICT} ${COMMANDS_LIST}
    DEPENDS ${BASE_DICT_PATH}
    WORKING_DIRECTORY ${CMAKE_CURRENT_BINARY_DIR}
    COMMENT "Generating ${ENCODING_NAME} dictionary")

  # Custom targeting for registration to the overall target
  add_custom_target(dict_${ENCODING_NAME} ALL DEPENDS ${OUT_DICT})
endfunction()

#------------------------------------------------------------------------------
# Generate dictionaries with character encodings.
add_dict_encoding_target("utf-8" "UTF-8" FALSE)
add_dict_encoding_target("euc-jp" "EUC-JISX0213" FALSE)
add_dict_encoding_target("cp932" "CP932" TRUE)
