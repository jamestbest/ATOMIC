cmake_minimum_required(VERSION 3.16.3)
project(ATOMIC C)

include(${CMAKE_SOURCE_DIR}/cmake/hadron.cmake)

set(CMAKE_C_STANDARD 99)

#add_link_options(-fsanitize=undefined,address)

add_compile_options(
        -Wall -Wextra -Wshadow -Wpointer-arith -Wcast-align -Wunused-parameter -Winit-self -Wswitch-enum
        -Wno-unused-parameter -Wno-unused-function -Wno-pedantic
        -ggdb3
#        -fsanitize=undefined,address
)

if (UNIX)
    add_link_options(-rdynamic)
endif (UNIX)

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
        Parser/Node.c
        Errors.c
        Parser/Parserr.c
        Parser/ShuntingYard.c
        SharedIncludes/Queue.c
        SharedIncludes/Stack.c
        Lexer/OpFolder.c
        Lexer/Lexerr.c
        SharedIncludes/Array.c

        Parser/StaticVerification.c

        InternalSettings.c
        Parser/TypeConstInfo.h
        Parser/StaticVerificationInternal.h

        SharedIncludes/Messages.c
        Lexer/Tokerr.c
        Lexer/Tokerr.h
        Generator/Generator.c
        Generator/Generator.h
        Generator/GeneratorInternal.h

        235/VM.c
)

add_hadron_support(ATOMICLI)

include_directories(${CMAKE_SOURCE_DIR})

add_executable(Flag_preprocessor
        FlagPreprocessor/FlagPreprocessor.c

        SharedIncludes/Vector.c
        SharedIncludes/Buffer.c
        SharedIncludes/Array.c

        SharedIncludes/Helper_String.c
        SharedIncludes/Helper_Math.c
        SharedIncludes/Helper_File.c

        SharedIncludes/Messages.c
        Commons.h
)

add_executable(Type_preprocessor
        TypePreprocessor/TypePreprocessor.c

        SharedIncludes/Buffer.c
        SharedIncludes/Helper_String.c
        SharedIncludes/Helper_File.c
        SharedIncludes/Helper_Math.c
        SharedIncludes/Messages.c
        SharedIncludes/Vector.c
        SharedIncludes/Array.c
        TypePreprocessor/TPPLexer.c
        TypePreprocessor/TPPLexer.h
        TypePreprocessor/TPPLexerInternal.h
        TypePreprocessor/TPPParser.c
        TypePreprocessor/TPPParser.h
        TypePreprocessor/TPPParserInternal.h
        Errors.c
        TypePreprocessor/TPPGenerator.c
        TypePreprocessor/TPPGenerator.h
        TypePreprocessor/TPPGeneratorInternal.h
)

add_hadron_support(Type_preprocessor)

# [[todo]] need to create a lib from these add_executable(Type_preprocessor TypePreprocessor/TypePreprocessor.c.c TypePreprocessor/TypePreprocessor.h)
add_executable(Testing
        Testing/TypeMatrix.c
        Testing/TypeMatrix.h
        TypePreprocessor/output/enum-temp.c
        TypePreprocessor/output/matrix-out.c
)
