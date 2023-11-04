cmake_minimum_required(VERSION 3.26)
project(ATOMIC C)

set(CMAKE_C_STANDARD 17)

add_executable(ATOMIC
        main.c
        Lexer.c
        Lexer.h
)
