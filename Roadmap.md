# ROADMAP

## MISC
 - [X] Create a roadmap
 - [X] Create a README

## CLI
 - [X] Create basic CLI
 - [X] Allow changing output elements
 - [X] Parse Flags
 - [X] Parse Options
 - [X] Parse input files

## CODE SCRAPER
Used to read in the file, checking for I/O errors

 - [X] Read in the file
 - [X] Display/record any I/O errors
 - [X] Split input into characters
 - [X] Pass data to the lexer

## LEXER
Process the characters to create base_tokens

 - [X] Create base_tokens
 - [X] lex comments
 - [X] lex strings
 - [X] lex numbers
 - [X] lex identifiers
 - [X] lex operators

## PARSER
 - [ ] generate an AST of the base_tokens
 - [ ] parse expressions
 - [ ] type checking
 - [ ] environments hold variables/functions 

## OPTIMIZER

## CODEGEN

## RUNTIME