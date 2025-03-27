include(CompilerFlags)

if (PROJECT_IS_TOP_LEVEL)
	set(DEFAULT_DISABLE_WARNINGS OFF)
else()
	set(DEFAULT_DISABLE_WARNINGS ON)
endif()
option(PV_DISABLE_WARNINGS "Whether to disable all warnings for this project" ${DEFAULT_DISABLE_WARNINGS})
option(PV_WARNINGS_AS_ERRORS "Whether to treat all warnings as errors" OFF)

function(set_internal_build_flags TARGET)
	set(DESIRED_FEATURES "ENABLE_MOST_WARNINGS")

	if (PV_WARNINGS_AS_ERRORS)
		list(APPEND DESIRED_FEATURES "ENABLE_WARNINGS_AS_ERRORS")
	endif()
	if (PV_DISABLE_WARNINGS)
		list(APPEND DESIRED_FEATURES "DISABLE_ALL_WARNINGS")
	endif()

	get_compiler_flags(
		${DESIRED_FEATURES}
		DISABLE_DEFAULT_FLAGS
		OUTPUT_VARIABLE COMPILER_FLAGS
	)

	target_link_libraries(${TARGET} PRIVATE "${COMPILER_FLAGS}")
endfunction()
