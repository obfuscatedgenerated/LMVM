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

## Addressing modes

| Symbol | Description                        | Example       | Definition of mode                                                                       |
|--------|------------------------------------|---------------|------------------------------------------------------------------------------------------|
| #      | Immediate                          | `ADD #5`      | Use the operand as the value                                                             |
| (none) | Direct                             | `ADD 5`       | Use the operand as an address to the value                                               |
| &      | Indirect                           | `ADD &5`      | Use the operand as an address to an address to the value                                 |
| LABEL  | Direct (described by DAT at LABEL) | `ADD fivevar` | Use the operand as a label for the address to the value (as declared in a DAT statement) |

> **Warning** The available memory addresses are 00-99 only. Any address outside of this range will cause an error.

## Using labels

Labels can be used as branching points, or as a name for a memory space when used with the DAT operation.

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