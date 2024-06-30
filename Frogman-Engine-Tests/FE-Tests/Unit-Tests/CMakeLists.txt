CMAKE_MINIMUM_REQUIRED(VERSION 3.27.0...3.29.6)
# Copyright © 2023~ UNKNOWN STRYKER. All Rights Reserved.
PROJECT(FE_UNIT_TEST
        VERSION 0.0
        DESCRIPTION "A group of executable unit test code binaries"
        LANGUAGES CXX)

INCLUDE("${CMAKE_CURRENT_SOURCE_DIR}/../../../Source/CMakeLists/predefined-settings.cmake")

INCLUDE_DIRECTORIES(${CMAKE_CURRENT_SOURCE_DIR}/../../../Third-Party/Tools/Google-Test/include)
INCLUDE_DIRECTORIES(${CMAKE_CURRENT_SOURCE_DIR}/../../../Third-Party/Tools/Benchmark/include)
INCLUDE_DIRECTORIES(${CMAKE_CURRENT_SOURCE_DIR}/../../../Source)
INCLUDE_DIRECTORIES(${CMAKE_CURRENT_SOURCE_DIR}/../../../Third-Party/Libraries/boost-1.85.0/include)
INCLUDE_DIRECTORIES(${CMAKE_CURRENT_SOURCE_DIR}/../../../Source/Third-Party/City-Hash)
INCLUDE_DIRECTORIES(${CMAKE_CURRENT_SOURCE_DIR}/../../../Source/Third-Party/Robin-Hood-Hash)

SET(THIRD_PARTY_CITY_HASH
	${CMAKE_CURRENT_SOURCE_DIR}/../../../Source/Third-Party/City-Hash/city.cc
	${CMAKE_CURRENT_SOURCE_DIR}/../../../Source/Third-Party/City-Hash/city.h
	${CMAKE_CURRENT_SOURCE_DIR}/../../../Source/Third-Party/City-Hash/citycrc.h
	)

SET(THIRD_PARTY_ROBIN_HOOD_HASH
	${CMAKE_CURRENT_SOURCE_DIR}/../../../Source/Third-Party/Robin-Hood-Hash/robin_hood.h
	)

IF(CMAKE_SYSTEM_NAME STREQUAL "Linux" AND TARGET_CPU_ARCHITECTURE STREQUAL "x86-64")

    SET(BOOST_STACKTRACE 
    $<$<CONFIG:DEBUG>:			${CMAKE_CURRENT_SOURCE_DIR}/../../../Third-Party/Libraries/boost-1.85.0/lib/linux/clang-10/debug/libboost_stacktrace_addr2line.a>
    $<$<CONFIG:RELWITHDEBINFO>: ${CMAKE_CURRENT_SOURCE_DIR}/../../../Third-Party/Libraries/boost-1.85.0/lib/linux/clang-10/release/libboost_stacktrace_addr2line.a>
    $<$<CONFIG:RELEASE>:		${CMAKE_CURRENT_SOURCE_DIR}/../../../Third-Party/Libraries/boost-1.85.0/lib/linux/clang-10/release/libboost_stacktrace_noop.a>
    $<$<CONFIG:MINSIZEREL>:		${CMAKE_CURRENT_SOURCE_DIR}/../../../Third-Party/Libraries/boost-1.85.0/lib/linux/clang-10/release/libboost_stacktrace_noop.a>
    )

    SET(BOOST_CHRONO 
    $<$<CONFIG:DEBUG>:			${CMAKE_CURRENT_SOURCE_DIR}/../../../Third-Party/Libraries/boost-1.85.0/lib/linux/clang-10/debug/libboost_chrono.a>
    $<$<CONFIG:RELWITHDEBINFO>: ${CMAKE_CURRENT_SOURCE_DIR}/../../../Third-Party/Libraries/boost-1.85.0/lib/linux/clang-10/release/libboost_chrono.a>
    $<$<CONFIG:RELEASE>:		${CMAKE_CURRENT_SOURCE_DIR}/../../../Third-Party/Libraries/boost-1.85.0/lib/linux/clang-10/release/libboost_chrono.a>
    $<$<CONFIG:MINSIZEREL>:		${CMAKE_CURRENT_SOURCE_DIR}/../../../Third-Party/Libraries/boost-1.85.0/lib/linux/clang-10/release/libboost_chrono.a>
    )


    SET(UNIT_TEST			
        $<$<CONFIG:DEBUG>:			${CMAKE_CURRENT_SOURCE_DIR}/../../../Third-Party/Tools/Google-Test/linux/libgtest.a>
        $<$<CONFIG:RELWITHDEBINFO>: ${CMAKE_CURRENT_SOURCE_DIR}/../../../Third-Party/Tools/Google-Test/linux/libgtest.a>
        $<$<CONFIG:RELEASE>:		${CMAKE_CURRENT_SOURCE_DIR}/../../../Third-Party/Tools/Google-Test/linux/libgtest.a>
        $<$<CONFIG:MINSIZEREL>:		${CMAKE_CURRENT_SOURCE_DIR}/../../../Third-Party/Tools/Google-Test/linux/libgtest.a>
    )

    ADD_COMPILE_OPTIONS(-DBENCHMARK_STATIC_DEFINE)
    SET(BENCHMARK_TEST			
        $<$<CONFIG:DEBUG>:			${CMAKE_CURRENT_SOURCE_DIR}/../../../Third-Party/Tools/Benchmark/linux/Debug/libbenchmark.a>
        $<$<CONFIG:RELWITHDEBINFO>: ${CMAKE_CURRENT_SOURCE_DIR}/../../../Third-Party/Tools/Benchmark/linux/Release/libbenchmark.a>
        $<$<CONFIG:RELEASE>:		${CMAKE_CURRENT_SOURCE_DIR}/../../../Third-Party/Tools/Benchmark/linux/Release/libbenchmark.a>
        $<$<CONFIG:MINSIZEREL>:		${CMAKE_CURRENT_SOURCE_DIR}/../../../Third-Party/Tools/Benchmark/linux/Release/libbenchmark.a>
    )

    SET(MEMORY
        $<$<CONFIG:DEBUG>:			${CMAKE_CURRENT_SOURCE_DIR}/../../../In-House/Libraries/memory/lib/libmemory.a>
        $<$<CONFIG:RELWITHDEBINFO>: ${CMAKE_CURRENT_SOURCE_DIR}/../../../In-House/Libraries/memory/lib/libmemory.a>
        $<$<CONFIG:RELEASE>:		${CMAKE_CURRENT_SOURCE_DIR}/../../../In-House/Libraries/memory/lib/libmemory.a>
        $<$<CONFIG:MINSIZEREL>:		${CMAKE_CURRENT_SOURCE_DIR}/../../../In-House/Libraries/memory/lib/libmemory.a>
    )

    
ELSEIF(CMAKE_SYSTEM_NAME STREQUAL "Windows" AND TARGET_CPU_ARCHITECTURE STREQUAL "x86-64")

    SET(BOOST_STACKTRACE 
    $<$<CONFIG:DEBUG>:			${CMAKE_CURRENT_SOURCE_DIR}/../../../Third-Party/Libraries/boost-1.85.0/lib/windows/clangw17-mt-x64/debug/libboost_stacktrace_windbg_cached-clangw17-mt-sgd-x64-1_85.lib>
    $<$<CONFIG:RELWITHDEBINFO>: ${CMAKE_CURRENT_SOURCE_DIR}/../../../Third-Party/Libraries/boost-1.85.0/lib/windows/clangw17-mt-x64/release/libboost_stacktrace_windbg_cached-clangw17-mt-s-x64-1_85.lib>
    $<$<CONFIG:RELEASE>:		${CMAKE_CURRENT_SOURCE_DIR}/../../../Third-Party/Libraries/boost-1.85.0/lib/windows/clangw17-mt-x64/release/libboost_stacktrace_noop-clangw17-mt-s-x64-1_85.lib>
    $<$<CONFIG:MINSIZEREL>:		${CMAKE_CURRENT_SOURCE_DIR}/../../../Third-Party/Libraries/boost-1.85.0/lib/windows/clangw17-mt-x64/release/libboost_stacktrace_noop-clangw17-mt-s-x64-1_85.lib>
    )

    SET(BOOST_CHRONO 
    $<$<CONFIG:DEBUG>:			${CMAKE_CURRENT_SOURCE_DIR}/../../../Third-Party/Libraries/boost-1.85.0/lib/windows/clangw17-mt-x64/debug/libboost_chrono-clangw17-mt-sgd-x64-1_85.lib>
    $<$<CONFIG:RELWITHDEBINFO>: ${CMAKE_CURRENT_SOURCE_DIR}/../../../Third-Party/Libraries/boost-1.85.0/lib/windows/clangw17-mt-x64/release/libboost_chrono-clangw17-mt-s-x64-1_85.lib>
    $<$<CONFIG:RELEASE>:		${CMAKE_CURRENT_SOURCE_DIR}/../../../Third-Party/Libraries/boost-1.85.0/lib/windows/clangw17-mt-x64/release/libboost_chrono-clangw17-mt-s-x64-1_85.lib>
    $<$<CONFIG:MINSIZEREL>:		${CMAKE_CURRENT_SOURCE_DIR}/../../../Third-Party/Libraries/boost-1.85.0/lib/windows/clangw17-mt-x64/release/libboost_chrono-clangw17-mt-s-x64-1_85.lib>
    )


    SET(UNIT_TEST			
        $<$<CONFIG:DEBUG>:			${CMAKE_CURRENT_SOURCE_DIR}/../../../Third-Party/Tools/Google-Test/windows/Debug/gtest.lib>
        $<$<CONFIG:RELWITHDEBINFO>: ${CMAKE_CURRENT_SOURCE_DIR}/../../../Third-Party/Tools/Google-Test/windows/RelWithDebInfo/gtest.lib>
        $<$<CONFIG:RELEASE>:		${CMAKE_CURRENT_SOURCE_DIR}/../../../Third-Party/Tools/Google-Test/windows/Release/gtest.lib>
        $<$<CONFIG:MINSIZEREL>:		${CMAKE_CURRENT_SOURCE_DIR}/../../../Third-Party/Tools/Google-Test/windows/MinSizeRel/gtest.lib>
    )

    ADD_COMPILE_OPTIONS(/DBENCHMARK_STATIC_DEFINE)
    LINK_LIBRARIES(advapi32.lib shlwapi.lib)
    SET(BENCHMARK_TEST			
        $<$<CONFIG:DEBUG>:			${CMAKE_CURRENT_SOURCE_DIR}/../../../Third-Party/Tools/Benchmark/windows/Debug/benchmark.lib>
        $<$<CONFIG:RELWITHDEBINFO>: ${CMAKE_CURRENT_SOURCE_DIR}/../../../Third-Party/Tools/Benchmark/windows/RelWithDebInfo/benchmark.lib>
        $<$<CONFIG:RELEASE>:		${CMAKE_CURRENT_SOURCE_DIR}/../../../Third-Party/Tools/Benchmark/windows/Release/benchmark.lib>
        $<$<CONFIG:MINSIZEREL>:		${CMAKE_CURRENT_SOURCE_DIR}/../../../Third-Party/Tools/Benchmark/windows/MinSizeRel/benchmark.lib>
    )

ENDIF()


SET(FE_CORE_PREREQUISITES
${CMAKE_CURRENT_SOURCE_DIR}/../../../Source/FE/core/macros/macro_definitions.h
${CMAKE_CURRENT_SOURCE_DIR}/../../../Source/FE/core/macros/attributes.h
${CMAKE_CURRENT_SOURCE_DIR}/../../../Source/FE/core/types.hxx 
${CMAKE_CURRENT_SOURCE_DIR}/../../../Source/FE/core/private/debug.h
)
ADD_SUBDIRECTORY(FE.core.prerequisites)

SET(FE_CORE_TYPE_TRAITS ${CMAKE_CURRENT_SOURCE_DIR}/../../../Source/FE/core/type_traits.hxx)
ADD_SUBDIRECTORY(FE.core.type_traits)

SET(FE_CORE_ALGORITHM_UTILITY 
${CMAKE_CURRENT_SOURCE_DIR}/../../../Source/FE/core/algorithm/utility.hxx
)
ADD_SUBDIRECTORY(FE.core.algorithm.utility)  

SET(FE_CORE_HASH
${CMAKE_CURRENT_SOURCE_DIR}/../../../Source/FE/core/hash.hpp
${CMAKE_CURRENT_SOURCE_DIR}/../../../Source/FE/core/private/hash.cpp
)
ADD_SUBDIRECTORY(FE.core.hash)

SET(FE_CORE_LOG
${CMAKE_CURRENT_SOURCE_DIR}/../../../Source/FE/core/log/macros.h
${CMAKE_CURRENT_SOURCE_DIR}/../../../Source/FE/core/log/logger.hpp
${CMAKE_CURRENT_SOURCE_DIR}/../../../Source/FE/core/log/private/logger.cpp
${CMAKE_CURRENT_SOURCE_DIR}/../../../Source/FE/core/private/clock.cpp
${CMAKE_CURRENT_SOURCE_DIR}/../../../Source/FE/core/log/format_string.h
${CMAKE_CURRENT_SOURCE_DIR}/../../../Source/FE/core/log/private/format_string.cpp
${CMAKE_CURRENT_SOURCE_DIR}/../../../Source/FE/core/private/thread.cpp
)
ADD_SUBDIRECTORY(FE.core.log)

SET(FE_CORE_THREAD
${CMAKE_CURRENT_SOURCE_DIR}/../../../Source/FE/core/thread.hpp
${CMAKE_CURRENT_SOURCE_DIR}/../../../Source/FE/core/private/thread.cpp
)
ADD_SUBDIRECTORY(FE.core.thread)

SET(FE_CORE_ALGORITHM_MATH 
${CMAKE_CURRENT_SOURCE_DIR}/../../../Source/FE/core/algorithm/math.h
${CMAKE_CURRENT_SOURCE_DIR}/../../../Source/FE/core/algorithm/private/math.cpp
)
ADD_SUBDIRECTORY(FE.core.algorithm.math)   

SET(FE_CORE_ITERATOR
${CMAKE_CURRENT_SOURCE_DIR}/../../../Source/FE/core/iterator.hxx
)
ADD_SUBDIRECTORY(FE.core.iterator)  

SET(FE_CORE_MEMORY
${CMAKE_CURRENT_SOURCE_DIR}/../../../Source/FE/core/memory.hxx
)
ADD_SUBDIRECTORY(FE.core.memory)   

SET(FE_CORE_ALGORITHM_STRING
${CMAKE_CURRENT_SOURCE_DIR}/../../../Source/FE/core/algorithm/string.hxx
)
ADD_SUBDIRECTORY(FE.core.algorithm.string)  

SET(FE_CORE_FSTRING
${CMAKE_CURRENT_SOURCE_DIR}/../../../Source/FE/core/fstring.hxx
)
ADD_SUBDIRECTORY(FE.core.fstring)

SET(FE_CORE_POOL
${CMAKE_CURRENT_SOURCE_DIR}/../../../Source/FE/core/private/pool_common.hxx
${CMAKE_CURRENT_SOURCE_DIR}/../../../Source/FE/core/pool.hxx
${CMAKE_CURRENT_SOURCE_DIR}/../../../Source/FE/core/block_pool.hxx
${CMAKE_CURRENT_SOURCE_DIR}/../../../Source/FE/core/pool_allocator.hxx
${CMAKE_CURRENT_SOURCE_DIR}/../../../Source/FE/core/block_pool_allocator.hxx
${CMAKE_CURRENT_SOURCE_DIR}/../../../Source/FE/core/private/allocator_base.hpp
${CMAKE_CURRENT_SOURCE_DIR}/../../../Source/FE/core/private/allocator_base.cpp
${CMAKE_CURRENT_SOURCE_DIR}/../../../Source/FE/core/private/clock.cpp
${CMAKE_CURRENT_SOURCE_DIR}/../../../Source/FE/core/private/hash.cpp
)
ADD_SUBDIRECTORY(FE.core.pool)

SET(FE_CORE_MANAGED
${CMAKE_CURRENT_SOURCE_DIR}/../../../Source/FE/core/managed/exclusive_ptr.hxx
${CMAKE_CURRENT_SOURCE_DIR}/../../../Source/FE/core/managed/ptr.hxx
${CMAKE_CURRENT_SOURCE_DIR}/../../../Source/FE/core/managed/unique_ptr.hxx

${CMAKE_CURRENT_SOURCE_DIR}/../../../Source/FE/core/managed/trackable.hxx
${CMAKE_CURRENT_SOURCE_DIR}/../../../Source/FE/core/managed/ref.hxx

${CMAKE_CURRENT_SOURCE_DIR}/../../../Source/FE/core/managed/private/ref_table.hpp
${CMAKE_CURRENT_SOURCE_DIR}/../../../Source/FE/core/managed/private/ref_table.cpp
)
ADD_SUBDIRECTORY(FE.core.managed)

SET(FE_CORE_STRING
${CMAKE_CURRENT_SOURCE_DIR}/../../../Source/FE/core/string.hxx
)
ADD_SUBDIRECTORY(FE.core.string)

SET(FE_CORE_STRING_VIEW
${CMAKE_CURRENT_SOURCE_DIR}/../../../Source/FE/core/string_view.hxx
)
ADD_SUBDIRECTORY(FE.core.string_view)

SET(FE_CORE_CONTAINERS
${CMAKE_CURRENT_SOURCE_DIR}/../../../Source/FE/core/stack.hxx
${CMAKE_CURRENT_SOURCE_DIR}/../../../Source/FE/core/queue.hxx
${CMAKE_CURRENT_SOURCE_DIR}/../../../Source/FE/core/array.hxx
${CMAKE_CURRENT_SOURCE_DIR}/../../../Source/FE/core/private/array_impl.hxx
)
ADD_SUBDIRECTORY(FE.core.containers)

SET(FE_FRAMEWORK_REFLECTION
${CMAKE_CURRENT_SOURCE_DIR}/../../../Source/FE/framework/reflection/reflection.h
${CMAKE_CURRENT_SOURCE_DIR}/../../../Source/FE/framework/reflection/function_table.hpp
${CMAKE_CURRENT_SOURCE_DIR}/../../../Source/FE/framework/reflection/private/function_table.cpp
${CMAKE_CURRENT_SOURCE_DIR}/../../../Source/FE/framework/reflection/variable_map.hpp
${CMAKE_CURRENT_SOURCE_DIR}/../../../Source/FE/framework/reflection/private/variable_map.cpp
)
ADD_SUBDIRECTORY(FE.framework.reflection)

SET(FE_FRAMEWORK_PREDEFINED_MAIN
${CMAKE_CURRENT_SOURCE_DIR}/../../../Source/FE/framework/framework.hpp
${CMAKE_CURRENT_SOURCE_DIR}/../../../Source/FE/framework/private/framework.cpp
)
ADD_SUBDIRECTORY(FE.framework.predefined_main)

SET(FE_CORE_ADJACENCY_GRAPH
${CMAKE_CURRENT_SOURCE_DIR}/../../../Source/FE/core/private/allocator_base.hpp
${CMAKE_CURRENT_SOURCE_DIR}/../../../Source/FE/core/private/allocator_base.cpp
${CMAKE_CURRENT_SOURCE_DIR}/../../../Source/FE/core/adjacency_graph.hxx
)
ADD_SUBDIRECTORY(FE.core.adjacency_graph)