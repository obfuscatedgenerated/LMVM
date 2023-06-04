# Little Man Virtual Machine

> Did you mean: [LLVM](https://llvm.org/)? 🙄

**A compiler + virtual machine to execute LMC assembly on your computer.**

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

## Building from source

1. Clone the repository: `git clone https://github.com/obfuscatedgenerated/LMVM.git`
2. Change directory: `cd LMVM`
3. Create a build directory: `mkdir build`
4. Change directory: `cd build`
5. Run CMake generation: `cmake ..` (you can force a specific generator with the `-G <generator>` option, enable release mode with `-DCMAKE_BUILD_TYPE=Release`)
6. Build the project: `cmake --build .` (you can specify a specific target with the `--target <target>` option)
7. Optional: create installers with CPack: `cpack` (you can specify a specific generator with the `-G <generator>` option, enable release mode with `-C Release`)
