PUSH BX
CALL main
HLT

main:
PUSH 10
POP [BX+0]
PUSH BX
PUSH [BX+0]
PUSH BX
PUSH 2
ADD
POP BX
CALL fact
POP [BX+1]
PUSH [BX+1]
OUT
PUSH 0
POP RX
POP BX
PUSH RX
RET
fact:
POP [BX+0]
PUSH 1
SQRT
POP [BX+1]
fact_while_0_check_cond:
PUSH [BX+0]
PUSH 0
JA fact_while_0_yes
JMP fact_while_0_no

fact_while_0_yes:
PUSH [BX+1]
PUSH [BX+0]
MUL
POP [BX+1]
PUSH [BX+0]
PUSH 1
SUB
POP [BX+0]
JMP fact_while_0_check_cond
fact_while_0_no:

PUSH [BX+1]
POP RX
POP BX
PUSH RX
RET
