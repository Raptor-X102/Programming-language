#ifndef CHECK_R_W_FLAGS
#define CHECK_R_W_FLAGS

#include <stdarg.h>
#include <stdio.h>
#include <string.h>

enum Check_flags_option {

    CHECK_R = 0x1,
    CHECK_W = 0x2,
    CHECK_R_W = 0x3
};

bool Check_r_w_flags(int check_option, char** argv, int argc, ...);

#endif
