cmake_minimum_required(VERSION 3.26)
project(ATOMIC C)

set(CMAKE_C_STANDARD 17)

add_executable(ATOMIC
        main.c
        Lexer.c
        Lexer.h
        main.h
        helper.h
        FLAGS.h FLAG_Shared.h)

add_executable(FLAG_PREPROCESS
        FLAG_PRE.c
        FLAG_PRE.h FLAG_Shared.h)
