#ifndef CHECK_R_W_FLAGS
#define CHECK_R_W_FLAGS

#include <stdarg.h>
#include <stdio.h>
#include <string.h>

struct file_types {

    char** read_files;
    char** write_files;
};

enum Check_flags_option {

    CHECK_R = 0x1,
    CHECK_W = 0x2,
    CHECK_R_W = 0x3
};

bool Check_r_w_flags(int check_option, char** argv, int argc, file_types* files);

#endif
