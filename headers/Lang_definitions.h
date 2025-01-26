#ifndef LANG_DEFS
#define LANG_DEFS

#include "..\Binary_Trees\Tree_definitions.h"

struct Node_data {

    int expression_type;
    int64_t value;
};

enum Expression_type {

    NUM = 0x1,
    VAR = 0x2,
    OP = 0x3,
    FUNCTION = 0x4,
    CONDITION = 0x5,
    SPECIAL_SYMBOL = 0xd15ab1ed,
    KEY_WORD = 0xC0DE,
    IF_BRANCHES = 0xBEAF
};

enum Operation_type_masks {

    Is_binary = 0x1
};

const int64_t ADD  = 0x1,
              SUB  = 0x3,
              MUL  = 0x5,
              DIV  = 0x7,
              POW  = 0x9,

              LN   = 0xC,
              SH   = 0x18,
              CH   = 0x1A,
              TH   = 0x1C,

              LOG  = 0x2,
              SIN  = 0x4,
              COS  = 0x6,
              TAN  = 0x8,
              COT  = 0xA,
              EXP  = 0xE,
              CTH  = 0x1E,
              ASH  = 0x20,
              ACH  = 0x22,
              ATH  = 0x24,
              ABS  = 0x2A,

              ASIN = 0x10,
              ACOS = 0x12,
              ATAN = 0x14,
              ACOT = 0x16,
              ACTH = 0x26,
              SQRT = 0x28,

              IF = 0xE0,
              WHILE = 0xE1,
              YES = 0xE2,
              NO = 0xE3,
              RETURN = 0xE4,

              LESS = 0xD0,
              LESS_OR_EQUAL = 0xD1,
              GREATER = 0xD2,
              GREATER_OR_EQUAL = 0xD3,
              EQUAL = 0xD4,
              NOT_EQUAL = 0xD5,

              PARENTHESES_OP = 0xF0,
              PARENTHESES_CL = 0xF1,
              SQUARE_BRACKET_OP = 0xF2,
              SQUARE_BRACKET_CL = 0xF3,
              BRACE_OP = 0xF4,
              BRACE_CL = 0xF5,
              ANGLE_BRACKET_OP = 0xF6,
              ANGLE_BRACKET_CL = 0xF7,
              EX_MARK = 0xF8,
              Q_MARK = 0xF9,
              ASSIGNMENT = 0xFA,
              COLON = 0xFB,
              SEMICOLON = 0xFC,
              COMMA = 0xFD,
              DOT = 0xFE,
              TAB = 0xF00,
              NEW_LINE = 0xF01,
              EOT = 0xFFFF,
              NULL_VALUE = 0x0;


const int64_t X_var = 'x',
              Y_var = 'y',
              Z_var = 'z';

#endif
