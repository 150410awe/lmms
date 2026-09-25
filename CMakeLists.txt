cmake_minimum_required(VERSION 3.20.0)

if(UNIX)
	set(CMAKE_C_COMPILER "/usr/bin/gcc" CACHE FILEPATH "C compiler" FORCE)
	set(CMAKE_CXX_COMPILER "/usr/bin/g++" CACHE FILEPATH "C++ compiler" FORCE)
elseif(WIN32)
	find_program(GCC_FOUND gcc)
	find_program(GXX_FOUND g++)
	
	if(GCC_FOUND AND GXX_FOUND)
        	message(STATUS "Found GCC: ${GCC_FOUND}")
        	message(STATUS "Found G++: ${GXX_FOUND}")
        	set(CMAKE_C_COMPILER "${GCC_FOUND}" CACHE FILEPATH "C compiler" FORCE)
        	set(CMAKE_CXX_COMPILER "${GXX_FOUND}" CACHE FILEPATH "C++ compiler" FORCE)
	elseif(DEFINED ENV{MSYSTEM})
        	message(STATUS "Detected MSYS2 environment")
		set(CMAKE_C_COMPILER "gcc.exe" CACHE FILEPATH "C compiler" FORCE)
		set(CMAKE_CXX_COMPILER "g++.exe" CACHE FILEPATH "C++ compiler" FORCE)
	else()
		message(FATAL_ERROR "unsupported platform")
	endif()
else()
	message(FATAL_ERROR "unsupported platform")
endif()

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS ON)

project(lmns VERSION 0.1.0 LANGUAGES CXX C)
include(GNUInstallDirs)
include(CMakePackageConfigHelpers)

option(LMNS_BUILD_EXAMPLES "build examples" ON)
option(LMNS_BUILD_TESTS "build tests" ON)
option(LMNS_BUILD_TOOLS "build tools" ON)

if(NOT CMAKE_CONFIGURATION_TYPES AND NOT CMAKE_BUILD_TYPE)
	set(CMAKE_BUILD_TYPE Debug CACHE STRING "Choose the type of build." FORCE)
endif()
message(STATUS "Build type: ${CMAKE_BUILD_TYPE}")

cmake_host_system_information(RESULT AVAIL_MEM QUERY AVAILABLE_PHYSICAL_MEMORY)
message(STATUS "Available physical memory: ${AVAIL_MEM}")

if(AVAIL_MEM LESS 2048) 
	message(FATAL_ERROR "system memory is not enough")
endif()
math(EXPR JOBS "${AVAIL_MEM} / 2048")
message(STATUS "Expected number of parallel compilations: ${JOBS}")

cmake_host_system_information(RESULT CPU_LOG QUERY NUMBER_OF_LOGICAL_CORES)
message(STATUS "Number of logical CPU cores: ${CPU_LOG}")

if(JOBS GREATER CPU_LOG)
	set(JOBS ${CPU_LOG})
endif()
message(STATUS "Parallel compile count: ${JOBS}")

math(EXPR REMAIN_MEM "${AVAIL_MEM} - ${JOBS} * 2048")
message(STATUS "Remaining memory: ${REMAIN_MEM}")
# TODO 计算出剩余内存, 给后面开启内存密集型功能做准备... 我还没写...
# 每个核心需要的内存
# 并行编译任务 2GB
# LTO (链接时优化) 4GB
# Unity Build (统一构建) 6GB
# AddressSanitizer 3倍
# Debug 符号 (完整) 2GB
# Mold 链接器 2GB

include(cmake/lmns_options.cmake)

add_subdirectory(lmns)
if(LMNS_BUILD_TESTS)
	enable_testing()
	add_subdirectory(tests)
endif()
if(LMNS_BUILD_TOOLS)
	add_subdirectory(tools)
endif()




