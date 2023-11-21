cmake_minimum_required(VERSION 3.26)
project(ATOMIC C)

set(CMAKE_C_STANDARD 17)

add_executable(ATOMICLI
        Compiler.c
        Flags.c
        Lexer.c
        CLI.c

        SharedIncludes/Flag_shared.c
        SharedIncludes/Vector.c
        SharedIncludes/Buffer.c

        SharedIncludes/Helper_String.c
        SharedIncludes/Helper_Math.c
        SharedIncludes/Helper_File.c

        Errors.h
        Tokens.h
)

add_executable(Flag_preprocessor
        FLAG-PREPROCESSOR/Flag_Preprocessor.c
        FLAG-PREPROCESSOR/Flag_Preprocessor.h

        SharedIncludes/Vector.c
        SharedIncludes/Buffer.c
        SharedIncludes/Flag_shared.c

        SharedIncludes/Helper_String.c
        SharedIncludes/Helper_Math.c
        SharedIncludes/Helper_File.c
)

# todo need to create a lib from these