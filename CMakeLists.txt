cmake_minimum_required(VERSION 3.16.3)
project(ATOMIC C)

set(CMAKE_C_STANDARD 99)

#add_link_options(-fsanitize=undefined)

add_compile_options(-Wall -Wextra -Wshadow -Wpointer-arith -Wcast-align -Wunused-parameter -Winit-self
        -Wno-unused-parameter -ggdb3 -pedantic-errors -Wswitch-enum -Wno-unused-function -Wno-pedantic)

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

        Lexer/Tokens.c
        Lexer/Lexer.c
        Parser/Parser.c
        Parser/Parser.h
        Parser/Node.c
        Parser/Node.h
        Parser/Parserr.h
        Errors.c
        Parser/Parserr.c
        Parser/ShuntingYard.c
        Parser/ShuntingYard.h
        SharedIncludes/Queue.c
        SharedIncludes/Queue.h
        SharedIncludes/Stack.c
        SharedIncludes/Stack.h
        Lexer/OpFolder.c
        Lexer/OpFolder.h
        Lexer/Lexerr.c
        Commons.h
        SharedIncludes/Array.c
        SharedIncludes/Array.h

        Parser/StaticVerification.c
        Parser/StaticVerification.h

        InternalSettings.c
        InternalSettings.h
        Parser/StaticVerificationInternal.h
)

target_link_libraries(ATOMICLI ${CMAKE_SOURCE_DIR}/libraries/libhadron.a)

add_executable(Flag_preprocessor
        FLAG-PREPROCESSOR/Flag_Preprocessor.c
        FLAG-PREPROCESSOR/Flag_Preprocessor.h

        SharedIncludes/Vector.c
        SharedIncludes/Buffer.c
        SharedIncludes/Flag_shared.c

        SharedIncludes/Helper_String.c
        SharedIncludes/Helper_Math.c
        SharedIncludes/Helper_File.c
        Commons.h
)

# [[todo]] need to create a lib from these