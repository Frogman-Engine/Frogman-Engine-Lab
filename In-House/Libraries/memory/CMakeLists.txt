CMAKE_MINIMUM_REQUIRED(VERSION 3.27.0...3.29.6)
# Copyright © 2023~ UNKNOWN STRYKER. All Rights Reserved.
PROJECT(memory
        VERSION 0.0
        DESCRIPTION "This library is a part of the Frogman Engine Core, and it includes memcpy, memset, memmove implementation in assembly language."
        LANGUAGES CXX)

ENABLE_LANGUAGE(ASM-ATT)

INCLUDE_DIRECTORIES(${CMAKE_CURRENT_SOURCE_DIR}/../../../Source)

if(TARGET_ARCHITECTURE STREQUAL "x86-64")
        SET(MEMCPY
        ${CMAKE_CURRENT_SOURCE_DIR}/../../../Source/FE/asm/__x86_64_memcpy.h
        ${CMAKE_CURRENT_SOURCE_DIR}/../../../Source/FE/asm/__x86_64_memcpy.asm
        )
endif()

ADD_LIBRARY(${CMAKE_PROJECT_NAME} STATIC ${MEMCPY})

SET_TARGET_PROPERTIES(${CMAKE_PROJECT_NAME} PROPERTIES 
ARCHIVE_OUTPUT_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}/lib
)


ADD_SUBDIRECTORY(tests)