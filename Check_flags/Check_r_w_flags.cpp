#include "Check_r_w_flags.h"

bool Check_r_w_flags(int check_option, char** argv, int argc, file_types* files) {

    bool rf_flag_found = false;
    bool wf_flag_found = false;
    bool flags_check_succsess = false;

    for(int i = 0; i < argc; i++) {

        if(!strncmp(argv[i], "-rf", 3)) {

            int rf_index = argv[i][3] - '0';

            if(rf_index < 0)
                return false;

            files->read_files[rf_index] = argv[i+1];
            rf_flag_found = true;
        }

        if(!strncmp(argv[i], "-wf", 3)) {

            int wf_index = argv[i][3] - '0';

            if(wf_index < 0)
                return false;

            files->write_files[wf_index] = argv[i+1];
            wf_flag_found = true;
        }
    }

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
