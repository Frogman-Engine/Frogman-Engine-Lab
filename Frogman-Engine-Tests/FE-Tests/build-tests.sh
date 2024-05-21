#!/bin/bash

CURRENT_DIRECTORY=$(pwd)
CMAKE_DIRECTORY=$CURRENT_DIRECTORY/../../Third-Party-Tools/cmake-3.29.3-linux-x86_64/bin/cmake

BUILD_FILES_FOLDER_NAME=build


mkdir $BUILD_FILES_FOLDER_NAME
cd $BUILD_FILES_FOLDER_NAME

$CMAKE_DIRECTORY -DCMAKE_BUILD_TYPE=Debug -DCMAKE_CXX_STANDARD=17 ..

sudo apt-get install make
make -j4
cd ..