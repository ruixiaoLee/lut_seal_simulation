# Copyright (c) Microsoft Corporation. All rights reserved.
# Licensed under the MIT license.

cmake_minimum_required(VERSION 3.13)

project(SEALExamples VERSION 4.0.0 LANGUAGES CXX)

# Executable will be in bin
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_SOURCE_DIR}/bin)
set(SEAL_USE_CXX17 OFF)
set(CMAKE_BUILD_TYPE Release)

add_executable(makeTable makeTable.cpp)
add_executable(keyGeneration keyGeneration.cpp)
add_executable(checkResult checkResult.cpp)

# In order to compute two-input function
add_executable(makeInputTwo makeInputTwo.cpp)
add_executable(searchInputTwo searchInputTwo.cpp)
add_executable(makeQueryTwo makeQueryTwo.cpp)
add_executable(extractOutputTwo extractOutputTwo.cpp)

# In order to compute three-input function
add_executable(makeInputThree makeInputThree.cpp)
add_executable(searchInputThree searchInputThree.cpp)
add_executable(makeQueryThree makeQueryThree.cpp)
add_executable(extractOutputThree extractOutputThree.cpp)

# In order to compute single-input function
add_executable(makeInput makeInput.cpp)
add_executable(searchInput searchInput.cpp)
add_executable(makeQuery makeQuery.cpp)
add_executable(extractOutput extractOutput.cpp)

# In order to compute poly_appro
add_executable(ckks_2_swish ckks_2_swish.cpp)
add_executable(ckks_4_swish ckks_4_swish.cpp)
add_executable(ckks_2_relu ckks_2_relu.cpp)
add_executable(ckks_4_relu ckks_4_relu.cpp)
add_executable(ckks_6_relu ckks_6_relu.cpp)
add_executable(ckks_8_relu ckks_8_relu.cpp)
add_executable(ckks_6_swish ckks_6_swish.cpp)
add_executable(ckks_8_swish ckks_8_swish.cpp)


# Import Microsoft SEAL
find_package(SEAL 4.0.0 EXACT REQUIRED)

# Import OpenMP
find_package(OpenMP REQUIRED)
if(OpenMP_FOUND)
 set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} ${OpenMP_C_FLAGS}")
 set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} ${OpenMP_CXX_FLAGS}")
endif()

# Link Microsoft SEAL
target_link_libraries(makeTable SEAL::seal)
target_link_libraries(keyGeneration SEAL::seal)
target_link_libraries(checkResult SEAL::seal)

# In order to compute two-input fun
target_link_libraries(makeInputTwo SEAL::seal)
target_link_libraries(searchInputTwo SEAL::seal)
target_link_libraries(makeQueryTwo SEAL::seal)
target_link_libraries(extractOutputTwo SEAL::seal)

target_link_libraries(makeInputThree SEAL::seal)
target_link_libraries(searchInputThree SEAL::seal)
target_link_libraries(makeQueryThree SEAL::seal)
target_link_libraries(extractOutputThree SEAL::seal)
target_link_libraries(checkResult SEAL::seal)

target_link_libraries(makeInput SEAL::seal)
target_link_libraries(searchInput SEAL::seal)
target_link_libraries(makeQuery SEAL::seal)
target_link_libraries(extractOutput SEAL::seal)

target_link_libraries(ckks_2_swish SEAL::seal)
target_link_libraries(ckks_4_swish SEAL::seal)
target_link_libraries(ckks_2_relu SEAL::seal)
target_link_libraries(ckks_4_relu SEAL::seal)
target_link_libraries(ckks_6_relu SEAL::seal)
target_link_libraries(ckks_8_relu SEAL::seal)
target_link_libraries(ckks_6_swish SEAL::seal)
target_link_libraries(ckks_8_swish SEAL::seal)
