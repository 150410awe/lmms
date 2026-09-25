file(GLOB LMNS_TEST_FILE_SET CONFIGURE_DEPENDS "${CMAKE_CURRENT_SOURCE_DIR}/*.cpp" )

list(SORT LMNS_TEST_FILE_SET)

find_package(Threads REQUIRED)

set(LMNS_TEST_OUT_DIR "${PROJECT_SOURCE_DIR}/bin/tests")

foreach(TEST_FILE_PATH IN LISTS LMNS_TEST_FILE_SET)
	get_filename_component(TEST_FILE_NAME "${TEST_FILE_PATH}" NAME_WE)
	message(STATUS "test file path: ${TEST_FILE_PATH}")
	message(STATUS "test file name: ${TEST_FILE_NAME}")
	message(STATUS "test file build path: ${LMNS_TEST_OUT_DIR}/${CMAKE_BUILD_TYPE}/${TEST_FILE_NAME}")
	
	if(TARGET "${TEST_FILE_NAME}")
		message(FATAL_ERROR "Cannot create test target '${TEST_FILE_NAME}': a target with htat name already exists")
	endif()

	add_executable("${TEST_FILE_NAME}" "${TEST_FILE_PATH}")

	target_link_libraries("${TEST_FILE_NAME}" PRIVATE lmns Threads::Threads)

	set_target_properties("${TEST_FILE_NAME}" PROPERTIES RUNTIME_OUTPUT_DIRECTORY "${LMNS_TEST_OUT_DIR}/${CMAKE_BUILD_TYPE}")

	add_test(NAME "${TEST_FILE_NAME}" COMMAND "${TEST_FILE_NAME}")
endforeach()



