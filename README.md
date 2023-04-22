# Little Man Virtual Machine

> Did you mean: [LLVM](https://llvm.org/)? 🙄

**A virtual machine to interpret execute LMC assembly on your computer.**

*(Functionally similar to but not the same as bytecode VMs like JVM. It's more like an interpreter than a VM.)*

## Mnemonics

| Code | Mnemonic | Description                  |
|------|----------|------------------------------|
| 1xx  | `ADD`    | Add                          |
| 2xx  | `SUB`    | Subtract                     |
| 3xx  | `STA`    | Store into address           |
| 5xx  | `LDA`    | Load from address            |
| 6xx  | `BRA`    | Branch (always)              |
| 7xx  | `BRZ`    | Branch if zero               |
| 8xx  | `BRP`    | Branch if positive (or zero) |
| 901  | `INP`    | Input                        |
| 902  | `OUT`    | Output                       |
| 000  | `HLT`    | Halt (coffee break)          |
|      | `DAT`    | Data declaration             |
|      | `;`      | Comment                      |

### Extended instruction set

These instructions are unique to this implementation of the LMC.

You can enable them by starting the file with `; lmvm-ext`.

| Code | Mnemonic | Description                     |
|------|----------|---------------------------------|
| 903  | `ERR`    | Output to stderr                |
| 904  | `TXT`    | Toggle string input/output mode |
| 01x  | `SLP`    | Sleep for x milliseconds        |
|      | `CST`    | Constant declaration            |
|      | `IMP`    | Import file                     |

The `ERR` instruction acts just like `OUT`, except it outputs to stderr instead of stdout.

The `TXT` instruction toggles the string input/output mode. When enabled, the `INP` instruction will read the next character from stdin or #0 if the end of input is reached. The `OUT` instruction will push a char to the buffer, which will be displayed when #0 is outputted.

The `SLP` instruction will sleep for x milliseconds. To attain a longer delay, call it multiple times.

The `CST` pseudo-instruction is used to declare constants. It functions the same as `DAT`, except the value may not be changed.

The `IMP` pseudo-instruction imports another file, where the relative or absolute path to it is named in place of the operand. This is accomplished by simply inserting the contents of the file on the line where this instruction is found, except any `DAT` declarations, which are inserted at the bottom to keep the program valid.

> **Warning** There are no safeguards against circular imports. If you import a file that imports the current file, you will get an infinite loop. Additionally, no spaces are allowed in the file path.

## Addressing modes

| Symbol | Description                        | Example       | Definition of mode                                                                       |
|--------|------------------------------------|---------------|------------------------------------------------------------------------------------------|
| #      | Immediate                          | `ADD #5`      | Use the operand as the value                                                             |
| (none) | Direct                             | `ADD 5`       | Use the operand as an address to the value                                               |
| &      | Indirect                           | `ADD &5`      | Use the operand as an address to an address to the value                                 |
| LABEL  | Direct (described by DAT at LABEL) | `ADD fivevar` | Use the operand as a label for the address to the value (as declared in a DAT statement) |

> **Note** In a DAT statement, the operand will always be an immediate value, no matter how it is written (
> e.g. `var DAT 5` and `var DAT #5` are the same expression). It will only fail if the operand is a label.

> **Warning** The available memory addresses are 00-99 only. Any address outside of this range will cause an error.

## Using labels

Labels can be used as branching points, or as a name for a memory space when used with the DAT operation.

## Execution flags

| Flag | Description                                                                    |
|------|--------------------------------------------------------------------------------|
| -h   | Display help                                                                   |
| -v   | Display version                                                                |
| -i   | Launch an interactive shell                                                    |
| -D   | Enable debug mode                                                              |
| -E   | Enable extended instruction set (alternative to `; lmvm-ext`, not recommended) |
| -S   | Strict mode. Any warnings are treated as errors.                               |


## Example programs

### [Count to 5](examples/count_to_5.asm)

```asm
; Count to 5

loop    LDA count  ; load the value of count into the accumulator
        ADD #1
        OUT        ; output the value of the accumulator
        STA count  ; hold the value of the accumulator into count
        SUB #5     ; subtract 5 from the accumulator...
        BRZ end    ; ...to therefore stop execution if the count is 5 (ACC=0)
        BRA loop   ; else, branch to loop
end     HLT
        
        
count   DAT #0
```

### Import example (extended)

#### add_5.asm

```asm
    ADD #5
```

#### main.asm

```asm
; lmvm-ext

    LDA #10        ; the ACC holds 10
    OUT            ; output 10
    IMP add_5.asm  ; import the add_5.asm file
    OUT            ; now the ACC holds 15
    HLT
```

### Hello world (extended)

```asm
; lmvm-ext

    ; enable string input/output mode
    TXT
    
    ; push each ASCII character to the buffer
    LDA #72
    OUT
    LDA #101
    OUT
    LDA #108
    OUT
    LDA #108
    OUT
    LDA #111
    OUT
    LDA #44
    OUT
    LDA #32
    OUT
    LDA #119
    OUT
    LDA #111
    OUT
    LDA #114
    OUT
    LDA #108
    OUT
    LDA #100
    OUT
    LDA #33
    OUT
    
    ; output the buffer with the null terminator
    LDA #0
    OUT
    
    HLT
```
