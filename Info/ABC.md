# ATOM BYTE CODE
This is information on the outputted ATOM byte code (.abc files)

## Call frame  
d Arg  
  A0-Ax  
 Scratch space (if needed) for ops  
| &nbsp;S0-Sx  
| Return address

## Calling convention
arguments are listed in the CALL opcode after the function name
In the VM there is space on the call frame 

## OPCODES
Some opcodes have a size attribution, this is the size of the operation, i.e.
STORE8 is storing an 8 byte value, STORE4 a 4 byte value, ADD8 is adding two 8 
byte values and storing an 8 byte result

### CALL
CALL <function name> <args>
Function names are currently resolved at runtime using the `FUNCTION-LABEL-LINK` 
section
### STORE
