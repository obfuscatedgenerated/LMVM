# Little Man Virtual Machine

> Did you mean: [LLVM](https://llvm.org/)? 🙄

**A compiler + virtual machine to execute LMC assembly on your computer.**

**NOTE: this README references extended modes. This feature is not yet implemented. It is documented to serve as a reference for future work.**

## Executables

### [LMVM (virtual machine)](src/vm)
### [LMASM (assembler)](src/assembler)

## Mnemonics

| Code | Mnemonic | Description                  |
|------|----------|------------------------------|
| 1xx  | `ADD`    | Add                          |
| 2xx  | `SUB`    | Subtract                     |
| 3xx  | `STA`    | Store into address           |
| 4xx  |          | Unused                       |
| 5xx  | `LDA`    | Load from address            |
| 6xx  | `BRA`    | Branch (always)              |
| 7xx  | `BRZ`    | Branch if zero               |
| 8xx  | `BRP`    | Branch if positive (or zero) |
| 901  | `INP`    | Input                        |
| 902  | `OUT`    | Output                       |
| 000  | `HLT`    | Halt (coffee break)          |
|      | `DAT`    | Data declaration             |
|      | `;`      | Comment                      |

All mnemonics are case-insensitive.

<!-- TODO: possibly at OTC: 922 from peter higginson's lmc to the standard mnemonics -->

### Extended instruction set

These instructions are unique to this implementation of the LMC.

You can enable them by starting the file with `; lmvm-ext <version>`. The latest extended mode version is 1.

| Code | Mnemonic | Description                     |
|------|----------|---------------------------------|
| 903  | `ERR`    | Output to stderr                |
| 904  | `TXT`    | Toggle string input/output mode |
| 01x  | `SLP`    | Sleep for x milliseconds        |
|      | `CST`    | Constant declaration            |
|      | `IMP`    | Import file                     |

<!-- TODO: could use negatives for extended instructions -->
<!-- TODO: DIV? MUL? MOD? -->

The `ERR` instruction acts just like `OUT`, except it outputs to stderr instead of stdout.

The `TXT` instruction toggles the string input/output mode. When enabled, the `INP` instruction will read the next character from stdin or #0 if the end of input is reached. The `OUT` instruction will push a char to the buffer, which will be displayed when #0 is outputted.

The `SLP` instruction will sleep for x milliseconds. To attain a longer delay, call it multiple times.

The `CST` pseudo-instruction is used to declare constants. It functions the same as `DAT`, except the value may not be changed.

The `IMP` pseudo-instruction imports another file, where the relative or absolute path to it is named in place of the operand. This is accomplished by simply inserting the contents of the file on the line where this instruction is found, except any `DAT` declarations, which are inserted at the bottom to keep the program valid.

> **Warning** There are no safeguards against circular imports. If you import a file that imports the current file, you will get an infinite loop. Additionally, no spaces are allowed in the file path.

## Addressing modes

| Symbol | Description                                | Example       | Definition of mode                                                                       |
|--------|--------------------------------------------|---------------|------------------------------------------------------------------------------------------|
| (none) | Direct                                     | `ADD 5`       | Use the operand as an address to the value                                               |
| LABEL  | Direct (described by address/DAT at LABEL) | `ADD fivevar` | Use the operand as a label for the address to the value (as declared in a DAT statement) |

Labels **must** consist of only Latin letters (abc...xyz), and are case-sensitive.

> **Warning** The available memory addresses are 0-99 only. Any address outside of this range will cause an error.

> **Note** In a DAT statement, the operand is an immediate value. If a label is passed, its address will be used as the value. It is the initial value of the variable.

## Using labels

Labels can be used as branching points, or as a name for a memory space when used with the DAT operation.

## Execution arguments

<!-- TODO: either marker comment or command flag to force INP to be limited to 0-999. by default it can be between MIN_INT and MAX_INT -->
<!-- TODO: similar to also apply this to all ACC operations -->
<!-- ;lmvm-strict-INP ;lmvm-strict-ACC -->

### Assembler

The first positional argument is the input file to use as the entrypoint. It is **required**.<br />
Additionally, these arguments are available:

| Short arg  | Long arg         | Description                                                                                                       |
|------------|------------------|-------------------------------------------------------------------------------------------------------------------|
| -o \<file> | --output \<file> | Output file, defaults to the same file name as the input (but with executable extension) in the current directory |
| -h         | --help           | Display help                                                                                                      |
| -k         | --no-overwrite   | Keep the output file if it already exists. Refuses to overwrite.                                                  |
| -v         | --version        | Display version                                                                                                   |
| -d         | --debug          | Enable debug mode                                                                                                 |
| -s         | --strict         | Strict mode. Treat warnings as errors.                                                                            |
| -x         | --silent         | Silent mode. Don't print anything to stdout or stderr.                                                            |

<!-- TODO: option to allow large or negative operands -->

### Virtual machine

The first positional argument is the file to execute. It is **required**.<br />
Additionally, these arguments are available:

| Short arg  | Long arg           | Description               |
|------------|--------------------|---------------------------|
| -h         | --help             | Display help              |
| -v         | --version          | Display version           |
| -d         | --debug            | Enable debug mode         |
| -x         | --silent           | Silent mode. No output.   |

## Example programs

### [Count to 5](examples/count_to_5.lmasm)

```asm
; Count to 5

loop    LDA count  ; load the value of count into the accumulator
        ADD one
        OUT        ; output the value of the accumulator
        STA count  ; hold the value of the accumulator into count
        SUB five   ; subtract 5 from the accumulator...
        BRZ end    ; ...to therefore stop execution if the count is 5 (ACC=0)
        BRA loop   ; else, branch to loop
end     HLT
        
        
count   DAT 0
one     DAT 1
five    DAT 5
```

### Import example (extended)

#### add_5.lmasm

```asm
; lmvm-ext 1

    ADD #5
```

#### main.lmasm

```asm
; lmvm-ext 1

    LDA #10        ; the ACC holds 10
    OUT            ; output 10
    IMP add_5.asm  ; import the add_5.asm file
    OUT            ; now the ACC holds 15
    HLT
```

### Hello world (extended)

```asm
; lmvm-ext 1

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

## Building from source

1. Clone the repository: `git clone https://github.com/obfuscatedgenerated/LMVM.git`
2. Change directory: `cd LMVM`
3. Create a build directory: `mkdir build`
4. Change directory: `cd build`
5. Run CMake generation: `cmake ..` (you can force a specific generator with the `-G <generator>` option, enable release mode with `-DCMAKE_BUILD_TYPE=Release`)<br>
By default, CPack installer data will be generated. This requires pandoc to be installed. To disable installer generation, pass `-DINSTALLER=OFF` to CMake.
6. Build the project: `cmake --build .` (you can specify a specific target with the `--target <target>` option)
7. Optional: create installers with CPack: `cpack` (you can specify a specific generator with the `-G <generator>` option, enable release mode with `-C Release`)
