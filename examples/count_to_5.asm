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