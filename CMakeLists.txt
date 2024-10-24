cmake_minimum_required(VERSION 3.16.3)
project(ATOMIC C)

set(CMAKE_C_STANDARD 99)

#add_link_options(-fsanitize=undefined,address)

add_compile_options(
        -Wall -Wextra -Wshadow -Wpointer-arith -Wcast-align -Wunused-parameter -Winit-self -Wswitch-enum
        -Wno-unused-parameter -Wno-unused-function -Wno-pedantic
        -ggdb3
#        -fsanitize=undefined,address
)

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

target_compile_options(ATOMICLI BEFORE PUBLIC -include ${CMAKE_SOURCE_DIR}/libraries/hadron.h)
target_link_libraries(ATOMICLI ${CMAKE_SOURCE_DIR}/libraries/libhadron.a m ${CMAKE_SOURCE_DIR}/libraries/DbgHelp.Lib) # link with hadron and the math lib, and DbgHelp
target_include_directories(ATOMICLI PRIVATE ${CMAKE_SOURCE_DIR}/FLAG-PREPROCESSOR)
target_include_directories(ATOMICLI PRIVATE ${CMAKE_SOURCE_DIR}/SharedIncludes)

if (WIN32)
    add_custom_command(TARGET ATOMICLI
            POST_BUILD
            COMMAND ../cv2pdb.exe $<TARGET_FILE:ATOMICLI> $<TARGET_FILE_DIR:ATOMICLI>/tempPdb.exe $<TARGET_FILE_DIR:ATOMICLI>/$<TARGET_NAME:ATOMICLI>.pdb
    )
endif (WIN32)

add_executable(Flag_preprocessor
        FLAG-PREPROCESSOR/FlagPreprocessor.c

        SharedIncludes/Vector.c
        SharedIncludes/Buffer.c
        SharedIncludes/Array.c

        SharedIncludes/Helper_String.c
        SharedIncludes/Helper_Math.c
        SharedIncludes/Helper_File.c

        SharedIncludes/Messages.c
        Commons.h
        FLAG-PREPROCESSOR/FlagPreprocessorInternal.h
        FLAG-PREPROCESSOR/FlagPreprocessorInject.h
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

# [[todo]] need to create a lib from these add_executable(Type_preprocessor TypePreprocessor/TypePreprocessor.c.c TypePreprocessor/TypePreprocessor.h)
