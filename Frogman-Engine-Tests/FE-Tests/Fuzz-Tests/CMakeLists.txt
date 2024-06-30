CMAKE_MINIMUM_REQUIRED(VERSION 3.27.0...3.29.6)
# Copyright © 2023~ UNKNOWN STRYKER. All Rights Reserved.
PROJECT(FE_FUZZ_TEST
        VERSION 0.0
        DESCRIPTION "A group of executable fuzz test code binaries"
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
	${CMAKE_CURRENT_SOURCE_DIR}/../../../Source/Third-Party/Robin-Hood-Hash/robin_hood.h)


IF(CMAKE_SYSTEM_NAME STREQUAL "Linux")

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

    
ELSEIF(CMAKE_SYSTEM_NAME STREQUAL "Windows")

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


ENDIF()


SET(FE_CORE_PREREQUISITES
${CMAKE_CURRENT_SOURCE_DIR}/../../../Source/FE/miscellaneous/configuration.h
${CMAKE_CURRENT_SOURCE_DIR}/../../../Source/FE/core/macros/macro_definitions.h
${CMAKE_CURRENT_SOURCE_DIR}/../../../Source/FE/core/macros/attributes.h
${CMAKE_CURRENT_SOURCE_DIR}/../../../Source/FE/core/types.hxx 
${CMAKE_CURRENT_SOURCE_DIR}/../../../Source/FE/core/private/debug.h
)

SET(FE_CORE_TYPE_TRAITS ${CMAKE_CURRENT_SOURCE_DIR}/../../../Source/FE/core/type_traits.hxx)

SET(FE_CORE_ALGORITHM_UTILITY ${CMAKE_CURRENT_SOURCE_DIR}/../../../Source/FE/core/algorithm/utility.hxx)

SET(FE_CORE_HASH
${CMAKE_CURRENT_SOURCE_DIR}/../../../Source/FE/core/hash.hpp
${CMAKE_CURRENT_SOURCE_DIR}/../../../Source/FE/core/private/hash.cpp
)

SET(FE_CORE_LOG
${CMAKE_CURRENT_SOURCE_DIR}/../../../Source/FE/core/log/macros.h
${CMAKE_CURRENT_SOURCE_DIR}/../../../Source/FE/core/log/logger.hpp
${CMAKE_CURRENT_SOURCE_DIR}/../../../Source/FE/core/log/private/logger.cpp
${CMAKE_CURRENT_SOURCE_DIR}/../../../Source/FE/core/clock.hpp
${CMAKE_CURRENT_SOURCE_DIR}/../../../Source/FE/core/private/clock.cpp
${CMAKE_CURRENT_SOURCE_DIR}/../../../Source/FE/core/log/format_string.h
${CMAKE_CURRENT_SOURCE_DIR}/../../../Source/FE/core/log/private/format_string.cpp
${CMAKE_CURRENT_SOURCE_DIR}/../../../Source/FE/core/thread.hpp
${CMAKE_CURRENT_SOURCE_DIR}/../../../Source/FE/core/private/thread.cpp
)

SET(FE_CORE_THREAD
${CMAKE_CURRENT_SOURCE_DIR}/../../../Source/FE/core/thread.hpp
${CMAKE_CURRENT_SOURCE_DIR}/../../../Source/FE/core/private/thread.cpp
)

SET(FE_CORE_ALGORITHM_MATH 
${CMAKE_CURRENT_SOURCE_DIR}/../../../Source/FE/core/algorithm/math.h
${CMAKE_CURRENT_SOURCE_DIR}/../../../Source/FE/core/algorithm/private/math.cpp
)

SET(FE_CORE_ITERATOR ${CMAKE_CURRENT_SOURCE_DIR}/../../../Source/FE/core/iterator.hxx)

SET(FE_CORE_MEMORY ${CMAKE_CURRENT_SOURCE_DIR}/../../../Source/FE/core/memory.hxx)  

SET(FE_CORE_ALGORITHM_STRING ${CMAKE_CURRENT_SOURCE_DIR}/../../../Source/FE/core/algorithm/string.hxx)

SET(FE_CORE_FSTRING ${CMAKE_CURRENT_SOURCE_DIR}/../../../Source/FE/core/fstring.hxx)

SET(FE_CORE_POOL
${CMAKE_CURRENT_SOURCE_DIR}/../../../Source/FE/core/private/pool_common.hxx
${CMAKE_CURRENT_SOURCE_DIR}/../../../Source/FE/core/pool.hxx
${CMAKE_CURRENT_SOURCE_DIR}/../../../Source/FE/core/block_pool.hxx
${CMAKE_CURRENT_SOURCE_DIR}/../../../Source/FE/core/pool_allocator.hxx
${CMAKE_CURRENT_SOURCE_DIR}/../../../Source/FE/core/block_pool_allocator.hxx
${CMAKE_CURRENT_SOURCE_DIR}/../../../Source/FE/core/private/allocator_base.hpp
${CMAKE_CURRENT_SOURCE_DIR}/../../../Source/FE/core/private/allocator_base.cpp
${CMAKE_CURRENT_SOURCE_DIR}/../../../Source/FE/core/clock.hpp
${CMAKE_CURRENT_SOURCE_DIR}/../../../Source/FE/core/private/clock.cpp
${CMAKE_CURRENT_SOURCE_DIR}/../../../Source/FE/core/hash.hpp
${CMAKE_CURRENT_SOURCE_DIR}/../../../Source/FE/core/private/hash.cpp
)

SET(FE_CORE_MANAGED
${CMAKE_CURRENT_SOURCE_DIR}/../../../Source/FE/core/managed/exclusive_ptr.hxx
${CMAKE_CURRENT_SOURCE_DIR}/../../../Source/FE/core/managed/ptr.hxx
${CMAKE_CURRENT_SOURCE_DIR}/../../../Source/FE/core/managed/unique_ptr.hxx

${CMAKE_CURRENT_SOURCE_DIR}/../../../Source/FE/core/managed/trackable.hxx
${CMAKE_CURRENT_SOURCE_DIR}/../../../Source/FE/core/managed/ref.hxx

${CMAKE_CURRENT_SOURCE_DIR}/../../../Source/FE/core/managed/private/ref_table.hpp
${CMAKE_CURRENT_SOURCE_DIR}/../../../Source/FE/core/managed/private/ref_table.cpp
)

SET(FE_CORE_STRING ${CMAKE_CURRENT_SOURCE_DIR}/../../../Source/FE/core/string.hxx)

SET(FE_CORE_STRING_VIEW ${CMAKE_CURRENT_SOURCE_DIR}/../../../Source/FE/core/string_view.hxx)

SET(FE_CORE_CONTAINERS
${CMAKE_CURRENT_SOURCE_DIR}/../../../Source/FE/core/containers/stack.hxx
${CMAKE_CURRENT_SOURCE_DIR}/../../../Source/FE/core/containers/queue.hxx
${CMAKE_CURRENT_SOURCE_DIR}/../../../Source/FE/core/containers/array.hxx
${CMAKE_CURRENT_SOURCE_DIR}/../../../Source/FE/core/containers/private/array_impl.hxx
)

SET(FE_FRAMEWORK_REFLECTION
${CMAKE_CURRENT_SOURCE_DIR}/../../../Source/FE/framework/reflection/reflection.h
${CMAKE_CURRENT_SOURCE_DIR}/../../../Source/FE/framework/reflection/function_table.hpp
${CMAKE_CURRENT_SOURCE_DIR}/../../../Source/FE/framework/reflection/private/function_table.cpp
)

SET(FE_FRAMEWORK_PREDEFINED_MAIN
${CMAKE_CURRENT_SOURCE_DIR}/../../../Source/FE/framework/framework.hpp
${CMAKE_CURRENT_SOURCE_DIR}/../../../Source/FE/framework/private/framework.cpp
${CMAKE_CURRENT_SOURCE_DIR}/../../../Source/FE/core/clock.hpp
${CMAKE_CURRENT_SOURCE_DIR}/../../../Source/FE/core/private/clock.cpp
)


#https://llvm.org/docs/LibFuzzer.html?ref=jumpcrypto.com
#-fsanitize=address
#-fsanitize=memory
#-fsanitize=undefined
#-fsanitize=fuzzer
SET(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -fsanitize=fuzzer,undefined,address")




ADD_EXECUTABLE(FE.core.algorithm.string.count_chars
${FE_CORE_LOG}
tests/FE.core.algorithm.string.count_chars.cpp
)
SET_TARGET_PROPERTIES(FE.core.algorithm.string.count_chars PROPERTIES RUNTIME_OUTPUT_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}/binaries)
TARGET_LINK_LIBRARIES(FE.core.algorithm.string.count_chars ${BOOST_STACKTRACE} ${BOOST_CHRONO})


ADD_EXECUTABLE(FE.core.algorithm.string.capitalize
${FE_CORE_LOG}
tests/FE.core.algorithm.string.capitalize.cpp
)
SET_TARGET_PROPERTIES(FE.core.algorithm.string.capitalize PROPERTIES RUNTIME_OUTPUT_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}/binaries)
TARGET_LINK_LIBRARIES(FE.core.algorithm.string.capitalize ${BOOST_STACKTRACE} ${BOOST_CHRONO})


ADD_EXECUTABLE(FE.core.algorithm.string.to_lowercase
${FE_CORE_LOG}
tests/FE.core.algorithm.string.to_lowercase.cpp
)
SET_TARGET_PROPERTIES(FE.core.algorithm.string.to_lowercase PROPERTIES RUNTIME_OUTPUT_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}/binaries)
TARGET_LINK_LIBRARIES(FE.core.algorithm.string.to_lowercase ${BOOST_STACKTRACE} ${BOOST_CHRONO})


ADD_EXECUTABLE(FE.core.algorithm.string.capitalize_every_first_letter_of_words
${FE_CORE_LOG}
tests/FE.core.algorithm.string.capitalize_every_first_letter_of_words.cpp
)
SET_TARGET_PROPERTIES(FE.core.algorithm.string.capitalize_every_first_letter_of_words PROPERTIES RUNTIME_OUTPUT_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}/binaries)
TARGET_LINK_LIBRARIES(FE.core.algorithm.string.capitalize_every_first_letter_of_words ${BOOST_STACKTRACE} ${BOOST_CHRONO})


ADD_EXECUTABLE(FE.core.algorithm.string.length
${FE_CORE_LOG}
tests/FE.core.algorithm.string.length.cpp
)
SET_TARGET_PROPERTIES(FE.core.algorithm.string.length PROPERTIES RUNTIME_OUTPUT_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}/binaries)
TARGET_LINK_LIBRARIES(FE.core.algorithm.string.length ${BOOST_STACKTRACE} ${BOOST_CHRONO})