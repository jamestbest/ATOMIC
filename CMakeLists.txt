cmake_minimum_required(VERSION 3.26)
project(ATOMIC C)

set(CMAKE_C_STANDARD 17)

add_executable(ATOMICLI
        CLI.c
        CLI.h
        SharedIncludes/FLAG_Shared.h
)

add_executable(FLAG_PREPROCESS
        FLAG-PREPROCESSOR/FLAG_PRE.c
        FLAG-PREPROCESSOR/FLAG_PRE.h
        SharedIncludes/FLAG_Shared.h
)
