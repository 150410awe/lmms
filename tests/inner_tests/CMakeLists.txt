set(INNER_TEST_NAME test_log_buffer)
set(INNER_TEST_SRC "${CMAKE_CURRENT_SOURCE_DIR}/test_log_buffer.cpp")

message(STATUS "test file path: ${INNER_TEST_SRC}")
message(STATUS "test file name: ${INNER_TEST_NAME}")
message(STATUS "test file build path: ${LMNS_TEST_OUT_DIR}/${CMAKE_BUILD_TYPE}/${INNER_TEST_NAME}")

add_executable("${INNER_TEST_NAME}" "${INNER_TEST_SRC}")

target_link_libraries("${INNER_TEST_NAME}" PRIVATE lmns Threads::Threads)

target_include_directories("${INNER_TEST_NAME}" PRIVATE "${PROJECT_SOURCE_DIR}/lmns/src")

set_target_properties("${INNER_TEST_NAME}" PROPERTIES RUNTIME_OUTPUT_DIRECTORY "${LMNS_TEST_OUT_DIR}/${CMAKE_BUILD_TYPE}")

add_test(NAME "${INNER_TEST_NAME}" COMMAND "${INNER_TEST_NAME}")