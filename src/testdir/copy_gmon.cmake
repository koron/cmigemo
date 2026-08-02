if(EXISTS "${CMAKE_BINARY_DIR}/qspeed.gmon")
    file(COPY_FILE "${CMAKE_BINARY_DIR}/qspeed.gmon" "${CMAKE_BINARY_DIR}/gmon.out" RESULT result)
endif()
