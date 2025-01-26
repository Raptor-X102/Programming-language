PUSH BX
CALL main
HLT

main:
PUSH 0
PUSH BX
ADD
POP BX
PUSH 10
POP [BX+0]
PUSH BX
PUSH [BX+0] ; order!!!
PUSH 2
PUSH BX
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
PUSH [BX+0]
PUSH 1
JE fact_if_1_yes
JMP fact_if_1_no

fact_if_1_yes:
PUSH [BX+0]
POP RX
POP BX
PUSH RX
RET

fact_if_1_no:
PUSH BX
PUSH [BX+0]
PUSH 1
SUB
PUSH 1
PUSH BX
ADD
POP BX
CALL fact
PUSH [BX+0]
MUL
POP RX
POP BX
PUSH RX
RET
