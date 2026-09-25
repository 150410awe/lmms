add_library(lmns_options INTERFACE)

target_compile_options(lmns_options INTERFACE
	$<$<CXX_COMPILER_ID:GNU>:
		-Wall
		-Wextra
		-Wpedantic
		-fno-strict-aliasing
	>
)

target_compile_options(lmns_options INTERFACE
    $<$<AND:$<PLATFORM_ID:Linux>,$<CXX_COMPILER_ID:GNU>>:
        -fPIC
    >
)

target_link_options(lmns_options INTERFACE
    $<$<AND:$<PLATFORM_ID:Linux>,$<CXX_COMPILER_ID:GNU>>:
        -rdynamic 
    >
)

target_compile_definitions(lmns_options INTERFACE
	$<$<CONFIG:Debug>:LMNS_DEBUG>
)

target_compile_options(lmns_options INTERFACE
	$<$<CONFIG:Debug>:
		-O0
		-g3
		-ggdb
	>
	$<$<CONFIG:Release>:
		-DNDEBUG
		-O2
		-fno-strict-aliasing
	>
	$<$<CONFIG:ReleaseDebInfo>:
		-DNDEBUG
		-O2
		-g
		-fno-strict-aliasing
	>
)

option(ENABLE_COVERAGE "Enable code coverage instrumentation" OFF)

if(ENABLE_COVERAGE)
	target_compile_options(lmns_options INTERFACE
		$<$<CONFIG:Debug>:
			--coverage
		>
	)
	target_link_options(lmns_options INTERFACE
		$<$<CONFIG:Debug>:
			--coverage
		>
	)
endif()




