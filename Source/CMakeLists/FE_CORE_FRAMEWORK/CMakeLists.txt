CMAKE_MINIMUM_REQUIRED(VERSION 3.24.0...3.27.7)
SET(FE_CORE_FRAMEWORK "FE_CORE_FRAMEWORK")




SET(_FRAMEWORK_
	${CMAKE_CURRENT_SOURCE_DIR}/../../FE/framework/framework.hpp
	)
SOURCE_GROUP("FE\\framework" FILES ${_FRAMEWORK_})

SET(_FRAMEWORK_REFLECTION_
	${CMAKE_CURRENT_SOURCE_DIR}/../../FE/framework/reflection/function_table.hpp
	)
SOURCE_GROUP("FE\\framework\\reflection" FILES ${_FRAMEWORK_REFLECTION_})

SET(_FRAMEWORK_PRIVATE_
	${CMAKE_CURRENT_SOURCE_DIR}/../../FE/framework/private/framework.cpp
	${CMAKE_CURRENT_SOURCE_DIR}/../../FE/framework/private/function_table.cpp
	${CMAKE_CURRENT_SOURCE_DIR}/../../FE/framework/private/internal_functions.h
	)
SOURCE_GROUP("FE\\framework\\private" FILES ${_FRAMEWORK_PRIVATE_})


ADD_LIBRARY(${FE_CORE_FRAMEWORK} STATIC
	${_FRAMEWORK_}
	${_FRAMEWORK_REFLECTION_}
	${_FRAMEWORK_PRIVATE_}
)


if(CMAKE_SYSTEM_NAME STREQUAL "Linux")
	MESSAGE("Configurating The Build Environment for Linux X86-64 Distributions")


	if(GNU_CPP)
		TARGET_COMPILE_OPTIONS(${FE_CORE_FRAMEWORK} PRIVATE
			$<$<CONFIG:DEBUG>:			${DEBUG_COMPILE_OPTIONS} 			-mavx>
			$<$<CONFIG:RELWITHDEBINFO>: ${RELWITHDEBINFO_COMPILE_OPTIONS} 	-mavx>
			$<$<CONFIG:RELEASE>:		${RELEASE_COMPILE_OPTIONS} 			-mavx>
			$<$<CONFIG:MINSIZEREL>:		${MINSIZEREL_COMPILE_OPTIONS} 		-mavx>
		)
	elseif(CLANG_CPP)
		TARGET_COMPILE_OPTIONS(${FE_CORE_FRAMEWORK} PRIVATE
			$<$<CONFIG:DEBUG>:			${DEBUG_COMPILE_OPTIONS} 			-mavx>
			$<$<CONFIG:RELWITHDEBINFO>: ${RELWITHDEBINFO_COMPILE_OPTIONS} 	-mavx>
			$<$<CONFIG:RELEASE>:		${RELEASE_COMPILE_OPTIONS} 			-mavx>
			$<$<CONFIG:MINSIZEREL>:		${MINSIZEREL_COMPILE_OPTIONS} 		-mavx>
		)
	endif()


	SET_TARGET_PROPERTIES(${FE_CORE_FRAMEWORK} PROPERTIES
        ARCHIVE_OUTPUT_DIRECTORY_DEBUG 			${CMAKE_CURRENT_SOURCE_DIR}/Linux/debug/a
        ARCHIVE_OUTPUT_DIRECTORY_RELWITHDEBINFO ${CMAKE_CURRENT_SOURCE_DIR}/Linux/release_with_debug_info/a
        ARCHIVE_OUTPUT_DIRECTORY_RELEASE 		${CMAKE_CURRENT_SOURCE_DIR}/Linux/release/a
        ARCHIVE_OUTPUT_DIRECTORY_MINSIZEREL 	${CMAKE_CURRENT_SOURCE_DIR}/Linux/minimally_sized_release/a
    )


	TARGET_LINK_LIBRARIES(${FE_CORE_FRAMEWORK}
#		stdc++fs
		${TBB_LINUX}
		${TBB_MALLOC_LINUX}
		${BOOST_STACKTRACE_LINUX}
	)




elseif(CMAKE_SYSTEM_NAME STREQUAL "Windows")
	MESSAGE("Configurating The Build Environment for Windows X86-64 Distributions")


	TARGET_COMPILE_OPTIONS(${FE_CORE_FRAMEWORK} PRIVATE
        $<$<CONFIG:DEBUG>:			${DEBUG_COMPILE_OPTIONS} 			/arch:AVX>
        $<$<CONFIG:RELWITHDEBINFO>:	${RELWITHDEBINFO_COMPILE_OPTIONS} 	/arch:AVX>
        $<$<CONFIG:RELEASE>:		${RELEASE_COMPILE_OPTIONS} 			/arch:AVX>
        $<$<CONFIG:MINSIZEREL>:		${MINSIZEREL_COMPILE_OPTIONS} 		/arch:AVX>
    )


	SET_TARGET_PROPERTIES(${FE_CORE_FRAMEWORK} PROPERTIES
		ARCHIVE_OUTPUT_DIRECTORY_DEBUG 			${CMAKE_CURRENT_SOURCE_DIR}/Windows/debug/lib
        ARCHIVE_OUTPUT_DIRECTORY_RELWITHDEBINFO ${CMAKE_CURRENT_SOURCE_DIR}/Windows/release_with_debug_info/lib
        ARCHIVE_OUTPUT_DIRECTORY_RELEASE 		${CMAKE_CURRENT_SOURCE_DIR}/Windows/release/lib
        ARCHIVE_OUTPUT_DIRECTORY_MINSIZEREL 	${CMAKE_CURRENT_SOURCE_DIR}/Windows/minimally_sized_release/lib
    )


	TARGET_LINK_LIBRARIES(${FE_CORE_FRAMEWORK} 
		${TBB_WINDOWS}
		${TBB_MALLOC_WINDOWS}
		${BOOST_STACKTRACE_WINDOWS}

	)


endif()




#// To enable or disable a certain feature, modify the macro definition to true or false.
#// 
#// Modifying the macro will not immediately affect your project.
#// The binary files e.g. (.lib, .a, .dll, or .so) need to be rebuilt to apply changes that have been made.

#// Frogman Engine Memory Tracker generates a memory utilization report over frames at the end of an app process.
#// The output file can be visualized as a graph image via Frogman Engine Memory Utilization Analyzer.
#// Memory tracking routines involve atomic integral sum and add operations, and its tracking frequency can be dynamically adjusted without recompiling the Engine binary.
#// Excluding this feature can improve an app performance.

#// FE_ASSERT can be disabled by removing the corresponding macro to eliminate error-checking overhead.
#// Please NOTE that FE_ASSERT aborts when circumstances are unrecoverable and critical. Therefore, precise debugging should be placed first.


TARGET_COMPILE_DEFINITIONS(${FE_CORE_FRAMEWORK} PRIVATE
	$<$<CONFIG:DEBUG>:			TBB_PREVIEW_MEMORY_POOL _HAS_EXCEPTIONS=0 TBB_USE_EXCEPTIONS=0 BOOST_NO_EXCEPTIONS _DEBUG_					_ENABLE_SUSPECT_ _ENABLE_ABORT_IF_ _ENABLE_LOG_ _ENABLE_EXIT_ _ENABLE_FE_CHECK_ONCE_ > #_ENABLE_MEMORY_TRACKER_
	$<$<CONFIG:RELWITHDEBINFO>:	TBB_PREVIEW_MEMORY_POOL _HAS_EXCEPTIONS=0 TBB_USE_EXCEPTIONS=0 BOOST_NO_EXCEPTIONS _RELWITHDEBINFO_			_ENABLE_SUSPECT_ _ENABLE_ABORT_IF_ _ENABLE_LOG_ _ENABLE_EXIT_ _ENABLE_FE_CHECK_ONCE_ >
	$<$<CONFIG:RELEASE>:		TBB_PREVIEW_MEMORY_POOL _HAS_EXCEPTIONS=0 TBB_USE_EXCEPTIONS=0 BOOST_NO_EXCEPTIONS _RELEASE_				_ENABLE_EXIT_ >
	$<$<CONFIG:MINSIZEREL>:		TBB_PREVIEW_MEMORY_POOL _HAS_EXCEPTIONS=0 TBB_USE_EXCEPTIONS=0 BOOST_NO_EXCEPTIONS _RELEASE_ _MINSIZEREL_	_ENABLE_EXIT_ >
)