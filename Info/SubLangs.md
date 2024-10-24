# Atom Specific Sub-langs
ATOMIC contains a couple of sub langs namely the 
 - Flag Listing, Argument Control, and Input Determination Lang
 - Type and Operator Pairings Lang

## Flags Lang
The first language is used to determine the flags and options that the CLI accepts

Its syntax is as follows  
FLAG <FLAG-NAME> [true, false]   
OPTION <OPTION-NAME>  
ARG <*>? <ARG_NAME> TYPE <[STR, INT, UINT, BOOL, CHAR]> <FROM [a, b, c]>?

### FLAG Directive
The FLAG keyword is used to define a flag for the compiler, it simply takes a name and a default value

### OPTION Directive
The OPTION keyword creates a new option, it takes an option name

### ARG Directive
The ARG keyword is used to add an argument to the last defined option. It takes an argument name, and type. 
Optionally it also takes in a list of data that restricts input to match. It can also be starred meaning that 
the argument can appear any number of times

## Type Lang
