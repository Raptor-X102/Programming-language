#include "Check_r_w_flags.h"

bool Check_r_w_flags(int check_option, char** argv, int argc, ...) {

    va_list args;
    va_start(args, argc);

    bool rf_flag_found = false;
    bool wf_flag_found = false;
    bool flags_check_succsess = false;

    for(int i = 0; i < argc; i++) {

        if(!strcmp(argv[i], "-rf")) {

            char** input_file_name = va_arg(args, char** );
            *input_file_name = argv[i+1];
            rf_flag_found = true;
        }

        if(!strcmp(argv[i], "-wf")) {

            char** output_file_name = va_arg(args, char** );
            *output_file_name = argv[i+1];
            wf_flag_found = true;
        }
    }

    va_end(args);

    if(check_option & CHECK_R)
        if(rf_flag_found)
            flags_check_succsess = true;

    if(check_option & CHECK_W)
        if(wf_flag_found)
            flags_check_succsess = true;

        else
           flags_check_succsess = false;

    return flags_check_succsess;
}
