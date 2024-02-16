# ATOMIC
ATOM in C. Compiler for the ATOM lang  
Welcome to the 4th rendition of ATOM lang, 4th times the charm though right?

<details>
<summary>Other md files</summary>

>[RoadMap](./Roadmap.md)  
>[ExampleProgram](./Info/ExampleProgram.atom)  
>[SimpleExampleProgram](./Info/SimpleExample.atm)

</details>

<details>
<summary>Table of Contents</summary>

1. [Description](#description)
2. [CLI](#cli)
3. [Language](#language)
    1. [Intro](#intro)
    2. [Variables](#variables)
    3. [Types](#types)
        1. [Numerical](#numerical)
        2. [String](#string)
        3. [Boolean](#boolean)
        4. [Arrays](#arrays)
        5. [Null](#null)
    4. [Type Sizes](#type-sizes)
    5. [Type casting](#type-casting)
    6. [Functions and Procedures](#functions-and-procedures)
        1. [Entry Functions](#entry-functions)
        2. [Function Calls](#function-calls)
        3. [Return](#return)
    7. [Loops](#loops)
        1. [For](#for)
        2. [ForEach](#foreach)
        3. [While](#while)
        4. [Times](#times)
    8. [Operators](#operators)
        1. [Binary Arithmetic](#binary-arithmetic-operators)
        2. [Unary Arithmetic](#unary-arithmetic-operators)
        3. [Trinary operators](#trinary-operators)
        4. [Comparison](#comparison-operators)
        5. [Assignment](#assignment-operators)
        6. [Misc](#misc-operators)
    9. [Conditional Statements](#conditional-statements)
        1. [If](#if-statements)
        2. [Elif](#elif-statements)
        3. [Else](#else-statements)
    10. [Comments](#comments)
    11. [Pointers](#pointers)
    12. [Keywords](#keywords)
</details>

## Description
this is my (4th) attempt at a compiler!
it will be a language similar to C, written in C!

Current timeline estimate: 5 years (that may be a bit generous)

## CLI

`atom compile <file>`  
this is the simplest command, and will just compile the file

`atom compile <file> -o ast tok -e main`  
this will compile the file and output (in some format) the AST and tokens  
It has also specified the entry function as main using `-e main`

## Language
### Intro
This is the section that will describe the grammar of the language
with some examples.  
It is subject to change.

### Variables
`<varName> : <type> = <value>`  
This is the most basic var definition  
`<VarName> := <value>`  
If no type is given, the compiler will infer the type  
`<varName> : <type>`  
This is a variable declaration, it will not be initialized  
`<varName> = <value>`  
This is a variable assignment, it must have already been declared

### Types
#### Numerical
Numerical types are named (somewhat) based on their fancy set name
* For example, all +ve integers are in the set of natural numbers, so they are called `n<elem_count>`
* All floating point numbers are in the set of real numbers, so they are called `r<elem_count>`
* Integers break the rule as I don't want to use `z` so instead its `i<elem_count>`
* There is also an experimental type called `q<elem_count>` which is a rational number,
    * it would be used to represent numbers as a ratio of two numbers that would then not
      use floating point arithmetic but some kind of fraction arithmetic to try and mitigate
      floating point errors

#### String
I have not yet decided how strings should be represented

to define a string, it could be something like this:  
`<varName> : string = "Hello World"`  
or maybe  
`<varName> : s<bytes>`

#### Boolean
Boolean values are either `true` or `false`  
`<varName> : bool = true`

#### Arrays
There are three types of arrays, static, dynamic, and dynamicLink
##### Static
Static arrays are arrays that have a fixed elem_count they are defined like this:  
`<varName> : i4[<elem_count>?] = [<value1>, <value2>, ...]`
##### Dynamic
Dynamic arrays are arrays that can change elem_count they are defined like this:  
`<varName> : i4[..] = [<value1>, <value2>, ...]`
##### DynamicLink
see [link](./Info/dynamicLink.md)  
`<varName> : i4[*] = [<value1>, <value2>, ...]`

#### Null
In ATOM lang null is called `nav` which means `not a value`

### Type Sizes
Throughout the types I've used `<elem_count>` alot this is would be a
numerical value to show how many bytes the type uses e.g. `i4`
is a 4-byte integer and `r8` is an 8-byte floating point number

| bytes | i   | n   | r   | q   | b |
|-------|-----|-----|-----|-----|---|
| 1     | ❌   | ❌   | ❌   | ❌   | ✅ |
| 2     | ✅   | ✅   | ❌   | ❌   | ✅ |
| 4     | ✅   | ✅   | ✅   | ✅   | ✅ |
| 8     | ✅   | ✅   | ✅   | ✅   | ✅ |
| 10    | ❌   | ❌   | ✅   | ❌   | ❌ |
| 16    | ❌   | ❌   | ❌   | ✅   | ❌ |

### Type casting
Type casting uses the `as` keyword
For example
```atom
    var1 : i4 = 5
    var2 : r8 = var1 as r8
    var3 : i8 = var1 as i8 + var2 as i8
```

You can also use the `::` symbol to cast all variables to the type
E.g.
```atom
    var1 : i4 = 5
    var2 : r8 = var1 as r8
    var3 :: i8 = var1 + var2
    
    var4 : i4 = 12
    var5 : string = "Hello, this is my favorite number: "
    var6 :: string = var5 + var4
    var7 : string = var5 + var4 as string
```
You cannot use `::` when using implicit type assignment e.g.
`varA ::= 12 + 1.4` is not valid


### Functions and Procedures
Functions have a return type, procedures do not

``func <funcName> (<arg1> : <type>, <arg2> : <type>) : <returnType> {
<code>
}``

``proc <procName> (<arg1> : <type>, <arg2> : <type>) {
<code>
}``

#### Entry Functions
Functions can also be defined as the entry function this is done by
having the keyword entry before them  
``entry func main() : i4 {
<code>
}``  
If there is one entry function in the program, it will be selected by the compiler
as the entry point. If there are multiple entry points it will first check if the -e
flag has been used in the CLI, else it will ask the user through the CLI to select
an entry point

#### Function Calls
Function calls are done like this:  
`<funcName>(<arg1>, <arg2>, ...)`  
`<varName> := <funcName>(<arg1>, <arg2>, ...)`  
`<varName> : <type> = <funcName>(<arg1>, <arg2>, ...)`

#### Return
Functions can return a value using the `ret` keyword
```atom
func getFive() : i4 {
    ret 5
}
```

#### Recursion
Recursive functions in atom can, and should, be given the `rec` keyword  
These functions then gain the ability to use a recursive `header` section this is a section of code
that will be executed when and only when a function that is not itself calls it

example
```atom
    entry rec func fib(i : i4) : i4 {
        header {
            print("calculating fib of " + i)
        }
        if i <= 1 {
            ret i
        }
        ret fib(i - 1) + fib(i - 2)
    }
```
Is this a good/useful idea?... I've got no fucking clue



### Loops
There are four types of loops, [for](#for), [foreach](#foreach), [while](#while), and [times](#times)

All loops allow you to omit the brackets around the code after the keyword
this is, however, optional.  
All examples will omit them.

#### For
```atom
    for [<decl>] to [<end>] do [<step>] {
        <code>
    }
```
`[<decl>]` can be
1. `<varName> : <type> = <startValue>` e.g. `i : i4 = 0`
2. `<varName> := <startValue>` e.g. `i := 0`
3. `<varName> = <startValue>` e.g. `i = 0` i needs to have already been declared
4. `<varName> : <type>` e.g. `i : i4` this will just be set to `i : i4 = 0`
5. `<varName>` e.g. `i` i needs to have already been declared and initialized

`[<end>]` can be
1. `<expression>` e.g. `i < 10`
2. `<varName>` e.g. `i` i needs to have already been declared and initialized
3. `<value>` e.g. `10`

`[<step>]` can be
1. `<expression>` e.g. `i += 1`, `i -= 1`, `i *= 2`, `i /= 2`
2. `<>` If no expression is given the compiler will attempt to find the step  
   e.g., Using `i++` if `i := 0` is the start and `i < 10` is the end

Some full examples

1. `for i : i4 = 0 to i < 10 do i += 1 { ... }`
2. `for i to j do i += 1 { ... }`
3. `for i := 0 to 10 { ... }`

#### Foreach
```atom
    foreach <varName> in <arrayName> {
        <code>
    }
```

```atom
    foreach <varName> in <arrayName> with i {
        <code>
    }
```
`i` is the index of the array element

#### While
```atom
    while <expression> {
        <code>
    }
```

#### Times
```atom
    times <Value> {
        <code>
    }
```
A times statement will just loop the number of times specified by value  
e.g. `times 10 { ... }` will loop 10 times
```atom
    i := 12
    times i { 
        print("Hello world")
    }
```
this will print "Hello world" 12 times

### Operators
#### Binary Arithmetic Operators
| Operator    | Description         | Example          |
|-------------|---------------------|------------------|
| +           | Addition            | 1 + 2            |
| -           | Subtraction         | 1 - 2            |
| *           | Multiplication      | 1 * 2            |
| /           | Division            | 1 / 2            |
| %           | Modulus             | 1 % 2            |
| ^           | Power               | 1 ^ 2            |
| && / and    | Logical and         | true && false    |
| \|\| / or   | Logical or          | true \|\| false  |
| \|-\| / xor | Logical xor         | true \|-\| false |
| &           | Bitwise and         | 1 & 2            |
| \|          | Bitwise or          | 1 \| 2           |
| \|-         | Bitwise xor         | 1 \|- 2          |
| <<          | Bitwise shift left  | 1 << 2           |
| \>\>        | Bitwise shift right | 1 >> 2           |


#### Unary Arithmetic Operators
| Operator | Description    | Example |
|----------|----------------|---------|
| ! / not  | Logical not    | !1      |
| ~        | Bitwise not    | ~1      |
| ++       | Post Increment | i++     |
| --       | Post Decrement | i--     |
| ++       | Pre Increment  | ++i     |
| --       | Pre Decrement  | --i     |
| +        | Positive       | +1      |
| -        | Negative       | -1      |

#### Trinary Operators
| Operator | Description | Example   |
|----------|-------------|-----------|
| ? :      | cmovCC      | a ? b : c |

#### Comparison Operators
| Operator | Description           | Example |
|----------|-----------------------|---------|
| ==       | Equal                 | 1 == 2  |
| !=       | Not equal             | 1 != 2  |
| <        | Less than             | 1 < 2   |
| \>       | Greater than          | 1 > 2   |
| <=       | Less than or equal    | 1 <= 2  |
| \>=      | Greater than or equal | 1 >= 2  |

#### Assignment Operators
| Operator | Description | Example   |
|----------|-------------|-----------|
| =        | Assignment  | a = 1     |
| +=       | Add         | a += 1    |
| -=       | Subtract    | a -= 1    |
| *=       | Multiply    | a *= 1    |
| /=       | Divide      | a /= 1    |
| %=       | Modulus     | a %= 1    |
| ^=       | Power       | a ^= 1    |
| &=       | Bitwise and | a &= 1    |
| \|=      | Bitwise or  | a \|= 1   |
| <<=      | Shift left  | a <<= 1   |
| \>\>=    | Shift right | a \>\>= 1 |


#### Misc Operators
| Operator | Description                 | Example |
|----------|-----------------------------|---------|
| <>       | Swap operator               | a <> b  |
| ..       | Range                       | 1 .. 10 |
| :        | Type annotation             | a : i4  |
| ,        | multi assign, arg seperator | a, b, c |

### Conditional Statements
#### If statements
The basic format of an `if` is
`if (<expression>) { <code> }`

as with loops the brackets around the `<expression>` are optional

example
```atom
    i := 10
    alive := true
    
    if i > 0 and alive {
        print("Hello world")
    }
```

#### Elif statements
`elif (<expression>) { <code> }`

example
```atom
    i := 10
    alive := true
    
    if i > 0 and alive {
        print("Hello world!")
    }
    elif i <= 0 and alive {
        print("Hello world?")
    }
```

#### Else statements
`else { <code> }`

example
```atom
    i := 10
    alive := true
    
    if i > 0 and alive {
        print("Hello world!")
    }
    elif i <= 0 and alive {
        print("Hello world?")
    }
    else {
        print("...")
    }
```

#### Nested Conditionals

example
```atom
    a := 10
    b := 20
    
    if a > 0 {
        if b > 0 {
            print("a and b are greater than 0")
        }
    }
    
    if a > b and b > 0 {
        if a < 0 {
            print("output1")
        }
        else {
            print("output2")
        }
    } else {print("output3")}
```

### Comments
`¬` is used as the single line comment character
`¬*` is the multi line comment start character
`*¬` is the multi line comment end character

examples
```atom
    <code>
    ¬ This is a single line comment
    <code>
    ¬*  text in comment 
        This is a multi line comment
        with multiple lines
    *¬ <code>
    ¬* single line using multi line comment *¬ <code>
    <code>
```

### ~~Error Handling~~ [Removed]

Keywords `Attempt`, `Ack`, and `Always`
```atom
    Attempt {
        <code>
    }
    Ack <Error> {
        <code>
    }
    Always {
        <code>
    }
```
[Addendum] Error handling will now be done by the return value  
[maybe] or could it instead be where a function returns both a value and an error code?


### Pointers
Mostly based on C's style of pointers

`>` is the pointer type modifier
`*` is the pointer dereferencing operator
`&` is the address operator.

The pointer modifier is tied to the type

```atom
    i,j: >i4;   //both are i4 pointers
    i,>j: i4;   //not valid
```

example
```atom
    i : i4 = 10  
    p : >i4      // p is a pointer to an i4
    p = &i       // p is a pointer to i
    *p = 20      // i is now 20
    print(*p)    // prints 20
```

### Keywords
| Keyword   | Description                                                                      |
|-----------|----------------------------------------------------------------------------------|
| `if`      | if statement                                                                     |
| `elif`    | elif statement                                                                   |
| `else`    | else statement                                                                   |
| `for`     | for loop                                                                         |
| `to`      | used in for loops to specify the end condition                                   |
| `do`      | used in for loops to specify the code to execute                                 |
| `foreach` | foreach loop                                                                     |
| `in`      | used in foreach loops to specify the array to loop through                       |
| `while`   | while loop                                                                       |
| `times`   | times loop                                                                       |
| `with`    | used in foreach loops to specify the index variable                              |
| `brk`     | break statement                                                                  |
| `cont`    | continue statement                                                               |
| `entry`   | defines a possible entry point function                                          |
| `func`    | function definition                                                              |
| `proc`    | procedure definition                                                             |
| `ret`     | return statement                                                                 |
| `rec`     | used for recursive functions to allow header use                                 |
| `header`  | specifies a block of code that will only be executed when not called recursively |
