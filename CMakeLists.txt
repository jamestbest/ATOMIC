cmake_minimum_required(VERSION 3.26)
project(ATOMIC C)

set(CMAKE_C_STANDARD 17)

add_compile_options(-Wall -Wextra -pedantic-errors -Wswitch-enum)

add_compile_options(-pedantic-errors -Wall -Wextra -Wshadow -Wpointer-arith -Wcast-align -Wunused-parameter -Winit-self
        -Wno-unused-parameter)

add_executable(ATOMICLI
        Compiler.c
        Flags.c
        CLI.c

        SharedIncludes/Flag_shared.c
        SharedIncludes/Vector.c
        SharedIncludes/Buffer.c

        SharedIncludes/Helper_String.c
        SharedIncludes/Helper_Math.c
        SharedIncludes/Helper_File.c

        Tokens.c
        LexerTwo.c
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

# [[todo]] need to create a lib from these