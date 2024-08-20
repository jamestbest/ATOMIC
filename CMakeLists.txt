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

add_executable(Type_preprocessor
        TypePreprocessor/TypePreprocessor.c

        SharedIncludes/Buffer.c
        SharedIncludes/Helper_String.c
        SharedIncludes/Helper_File.c
        SharedIncludes/Messages.c
        SharedIncludes/Vector.c
        SharedIncludes/Array.c
        TypePreprocessor/TPPLexer.c
        TypePreprocessor/TPPLexer.h
        TypePreprocessor/TPPLexerInternal.h
        TypePreprocessor/TPPParser.c
        TypePreprocessor/TPPParser.h
        TypePreprocessor/TPPParserInternal.h

)

# [[todo]] need to create a lib from theseadd_executable(Type_preprocessor TypePreprocessor/TypePreprocessor.c.c TypePreprocessor/TypePreprocessor.h)
