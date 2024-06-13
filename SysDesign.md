# Overview of the design choices within ATOMIC
Mainly for reminding myself of my terrible design choices

## TOC


### CLI Flags
There are two ways to toggle flags
    1. Flags (--) these are single binary options, it's on or its off
    2. Options (-) these take in arguments e.g. -out ast tok (output the tokens and ast generated)

Flag information is held in an array  
There is a flag get function to retrieve the value of a flag

The flag preprocessor is used when adding new flags and produces the hashes and strings that are used when setting flags

#### Changes & thoughts
Here stands the greatest unneeded and least impactful optimization attempt I have made (so far). Within the CLI there needs
to be some way to process the given flags (--) and options (-) that are passed to the program, and so was created the Flags.c, 
Flag_Preprocessor.c, and Flag_shared.c

My initial plan was simple, hash the flags in some way and then use a switch-case to get that marvelous jump table magic to
turn a O(n) lookup into O(1), amazing. My data is not random, the flags are probably (first problem) unique to the first 8 characters, 
so I can wrap up those characters into a nice old 64-bit integer and let the compiler create a wonderful switch-case.

Of course everything has a cost, a jump tables is space, the array needs to be stored somewhere and so ideally the options have to
follow some kind of pattern. The simplest example would be switching on a simple enum that maybe has 4 entries, with values 0-3.
This is easy, just make an array with the place to jump, check the value given is in range and if so jump to arr[value]. No need
to compare the value to 0, then to 1, and 2, and 3, we just use the array. You can do this with more complicated patterns like 1,3,6,7,8,11
the compiler will just fill in the blanks with the out of range case (e.g. [.fail, .case1, .fail, .fail, .case3, .fail... etc].
However, as I mentioned it does have a cost say you had the cases 482197983 and 12838374 creating a jump table for this would be
less than space efficient, the best you can do is subtract the smallest and then have an array with the rest, and so in this case
it is without a doubt better to just use comparisons, if you have a lot of these cases that don't have a pattern then you can
use a decision tree instead of going linearly. There are also some interesting bit set optimisations some compilers do.

I'm sure by now a lot of people can see where this is going, packing the flag's string into an integer produces integers that follow
no pattern and so no jump table, the compiler can still make a search tree of the hashes, so it's better than a linear search, but
this is something I could have done with strcmp as its return value gives more information than equality.

Overall this means that adding a flag requires the use of the flag preprocessor which is rather annoying. The moral of the story is
naturally one of premature optimization and surface level knowledge of a feature like jump tables and switch-cases, but it was fun to
code and that's that I guess

### COCKs & COCOS
Within all compilers is the need to interact with the OS and Kernel, these are not usual function calls, they require 
specific interrupt calls

So enters the Compiler Outputted Call to Kernel and Compiler Outputted Calls to the Operating System