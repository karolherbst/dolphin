set(CMAKE_FAKELANG_CREATE_STATIC_LIBRARY "touch <TARGET>")
macro(add_dolphin_library lib srcs libs)
	add_library(${lib} STATIC ${srcs})
	target_link_libraries(${lib} ${libs})
	if(ENABLE_PCH)
		cotire(${lib})
	endif(ENABLE_PCH)
endmacro(add_dolphin_library)

macro(add_dolphin_library_nopch lib srcs libs)
	add_library(${lib} STATIC ${srcs})
	target_link_libraries(${lib} ${libs})
endmacro(add_dolphin_library_nopch)

macro(add_dolphin_executable exe srcs libs)
	add_executable(${exe} ${srcs})
	target_link_libraries(${exe} ${libs})
	if(ENABLE_PCH)
		cotire(${exe})
	endif(ENABLE_PCH)
endmacro(add_dolphin_executable)

add_subdirectory(Core)
add_subdirectory(UnitTests)

if (DSPTOOL)
	add_subdirectory(DSPTool)
endif()

# TODO: Add DSPSpy. Preferrably make it option() and cpack component
