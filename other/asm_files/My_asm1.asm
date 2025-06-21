PUSH BX
CALL main
HLT

main:
PUSH BX
PUSH [BX+0]
PUSH BX
PUSH 3
ADD
POP BX
CALL In
PUSH BX
PUSH [BX+1]
PUSH BX
PUSH 3
ADD
POP BX
CALL In
PUSH BX
PUSH [BX+2]
PUSH BX
PUSH 3
ADD
POP BX
CALL In
PUSH BX
PUSH [BX+2]
PUSH [BX+1]
PUSH [BX+0]
PUSH BX
PUSH 3
ADD
POP BX
CALL Solve_quad_eq
OUT
PUSH 0
POP RX
POP BX
PUSH RX
RET
Solve_quad_eq:
POP [BX+2]
POP [BX+1]
POP [BX+0]
PUSH 1e-06
POP [BX+3]
PUSH [BX+1]
PUSH [BX+1]
MUL
PUSH 4
PUSH [BX+0]
MUL
PUSH [BX+2]
MUL
SUB
POP [BX+4]
PUSH [BX+4]
PUSH 0
JB Solve_quad_eq_if_0_yes
JMP Solve_quad_eq_if_0_no

Solve_quad_eq_if_0_yes:
PUSH 0
POP RX
POP BX
PUSH RX
RET
JMP Solve_quad_eq_if_0_yes_ret
Solve_quad_eq_if_0_no:
Solve_quad_eq_if_0_yes_ret:
PUSH [BX+4]
SQRT
POP [BX+5]
PUSH [BX+0]
ABS
PUSH [BX+3]
JB Solve_quad_eq_if_1_yes
JMP Solve_quad_eq_if_1_no

Solve_quad_eq_if_1_yes:
PUSH BX
PUSH [BX+2]
PUSH [BX+1]
PUSH BX
PUSH 8
ADD
POP BX
CALL Solve_lin_eq
POP RX
POP BX
PUSH RX
RET
JMP Solve_quad_eq_if_1_yes_ret
Solve_quad_eq_if_1_no:
Solve_quad_eq_if_1_yes_ret:
PUSH [BX+4]
PUSH [BX+3]
JB Solve_quad_eq_if_2_yes
JMP Solve_quad_eq_if_2_no

Solve_quad_eq_if_2_yes:
PUSH -1
PUSH [BX+1]
MUL
PUSH 2
PUSH [BX+0]
MUL
DIV
POP [BX+6]
PUSH [BX+6]
OUT
PUSH 1
POP RX
POP BX
PUSH RX
RET
JMP Solve_quad_eq_if_2_yes_ret
Solve_quad_eq_if_2_no:
Solve_quad_eq_if_2_yes_ret:
PUSH -1
PUSH [BX+1]
MUL
PUSH [BX+5]
ADD
PUSH 2
PUSH [BX+0]
MUL
DIV
POP [BX+6]
PUSH -1
PUSH [BX+1]
MUL
PUSH [BX+5]
SUB
PUSH 2
PUSH [BX+0]
MUL
DIV
POP [BX+7]
PUSH [BX+6]
OUT
PUSH [BX+7]
OUT
PUSH 2
POP RX
POP BX
PUSH RX
RET
Solve_lin_eq:
POP [BX+1]
POP [BX+0]
PUSH 1e-06
POP [BX+2]
PUSH [BX+0]
ABS
PUSH [BX+2]
JB Solve_lin_eq_if_0_yes
JMP Solve_lin_eq_if_0_no

Solve_lin_eq_if_0_yes:
PUSH [BX+1]
ABS
PUSH [BX+2]
JB Solve_lin_eq_if_1_yes
JMP Solve_lin_eq_if_1_no

Solve_lin_eq_if_1_yes:
PUSH inf
POP RX
POP BX
PUSH RX
RET
JMP Solve_lin_eq_if_1_yes_ret
Solve_lin_eq_if_1_no:
PUSH 0
POP RX
POP BX
PUSH RX
RET
Solve_lin_eq_if_1_yes_ret:
JMP Solve_lin_eq_if_0_yes_ret
Solve_lin_eq_if_0_no:
PUSH -1
PUSH [BX+1]
MUL
PUSH [BX+0]
DIV
POP [BX+3]
PUSH [BX+3]
OUT
PUSH 1
POP RX
POP BX
PUSH RX
RET
Solve_lin_eq_if_0_yes_ret:
