#include "Assembler.h"

#define COMMANDS_DEF(name, cmd, arg_n, spu_func, asm_func) name, cmd, arg_n, asm_func

Commands_data Commands[] =
{

#include "Commands_data.h"

};

#undef COMMANDS_DEF

void Compile_code(const char* compiling_file, const char* out_file, Labels* labels) {

    uint64_t file_size = get_file_size(compiling_file); DEBUG_PRINTF("Size was gotten\n");

    FILE* input_file = fopen(compiling_file, "rb");
    if(!input_file) {

        DEBUG_PRINTF("ERROR: file was not opened");
        return;
    }

    uint64_t commands_amount = 0;

    FILE* output_file = fopen(out_file, "wb+");
    if(!output_file) {

        DEBUG_PRINTF("ERROR: file was not opened");
        return;
    }

    DEBUG_PRINTF("convertion started\n");
    commands_amount = Convert_txt_to_code(input_file, output_file, file_size, labels);
    DEBUG_PRINTF("commands_amount = %d", commands_amount);
    DEBUG_PRINTF("ended\n");

    fclose(input_file);
    fclose(output_file);
    DEBUG_PRINTF("output file was closed\n");
}

// fixme
/*char* Compiled_filename(const char* compiling_file) {

    size_t length = strlen(compiling_file);
    char* result_filename = (char*) calloc(length+5, sizeof(char));
    if(compiling_file[length-2] == 'a' &&
       compiling_file[length-1] == 's' &&
       compiling_file[length-0] == 'm') {

        int i = 0;
        for(; compiling_file[i] != '.'; i++)
            result_filename[i] = compiling_file[i];

        result_filename[i]   = '(';
        result_filename[i+1] = '1';
        result_filename[i+2] = ')';
        result_filename[i+3] = '.';
        result_filename[i+3] = 'a';
        result_filename[i+3] = 's';
        result_filename[i+3] = 'm';
    }
    else {

        int i = 0;
        for(; compiling_file[i] != '.'; i++)
            result_filename[i] = compiling_file[i];

        result_filename[i]   = '.';
        result_filename[i+1] = 'a';
        result_filename[i+2] = 's';
        result_filename[i+3] = 'm';
    }
    DEBUG_PRINTF("\n\nresult_filename = %s\n\n", result_filename);
    return result_filename;
}
*/
uint64_t Convert_txt_to_code(FILE* input_file, FILE* output_file, uint64_t code_length, Labels* labels) {

    char* tmp_ptr = (char*) calloc(code_length * 8, sizeof(char));
    char* code_data = NULL;
    if(tmp_ptr) {

        code_data = tmp_ptr;
        tmp_ptr = NULL;
    }

    else {

        DEBUG_PRINTF("ERROR: memory was not allocated\n");
        return 0;
    }

    char* input_buffer = NULL;
    tmp_ptr = (char*) calloc(code_length+10, sizeof(char));
    if(tmp_ptr) {

        input_buffer = tmp_ptr;
        tmp_ptr = NULL;
    }

    else {

        DEBUG_PRINTF("ERROR: memory was not allocated\n");
        return 0;
    }

/*********Read and write initial data************************************* */
    char signature[signature_len] = "RptrX";
    int version = 2;
    DEBUG_PRINTF("%s\n", signature);
    fwrite(signature, sizeof(char), strlen(signature), output_file);
    fwrite(&version, sizeof(int), 1, output_file);
    fread(input_buffer, sizeof(char), code_length, input_file);
//********************Reading commands from input file*********************
//********************Inserting data  */
    uint64_t curr_code_byte = 0, inword_pos = 0, curr_pos = 0;

    for(; input_buffer[curr_pos] != EOF &&
          curr_pos < code_length; curr_pos = inword_pos + 1) { // fixme: scanf

        bool cmd_found = false;
        Get_next_word(input_buffer, &curr_pos, &inword_pos, code_length);

        for(int i = 0; i < sizeof(Commands) / sizeof(Commands[0]); i++) {

            if(!strcmp(input_buffer+curr_pos, Commands[i].command_name)) { //Finding commands

                code_data[curr_code_byte] = (char) Commands[i].command_num;
                curr_code_byte++;

                if(Commands[i].args_amount > 0) {

                    curr_pos = inword_pos + 1;
                    Get_next_word(input_buffer, &curr_pos, &inword_pos, code_length);
                    Commands[i].Asm_Func(code_data, &curr_code_byte, input_buffer + curr_pos, labels);
                }

                ASM_DUMP(code_data, curr_code_byte);
                cmd_found = true;
            }
        }

        char* tmp_ch_ptr = strchr(input_buffer+curr_pos, ':'); //finding labels
        if(tmp_ch_ptr) {

            *tmp_ch_ptr = '\0';
            Label_handle(code_data, &curr_code_byte, input_buffer+curr_pos, labels);
        }

        else if(!cmd_found){ // finding numbers (for Draw and functions with more than 2 numbers as parametres)
            int64_t tmp_int = atoll(input_buffer+curr_pos);
            memcpy(&code_data[curr_code_byte], &tmp_int, sizeof(int64_t));
            curr_code_byte += sizeof(int64_t);
            DEBUG_PRINTF("    [%d]atoi = %d\n",curr_code_byte, tmp_int);
        }
    }

    //txDump(code_data);
    //writing data in output file
    fwrite(&curr_code_byte, sizeof(uint64_t), 1, output_file);
    fwrite(code_data, sizeof(char), curr_code_byte, output_file);

    return curr_code_byte;
}


void Get_next_word(char* input_buffer, uint64_t* curr_pos, uint64_t* inword_pos, uint64_t code_length) {

    DEBUG_PRINTF("\n\n");
    while(*curr_pos < code_length &&
          input_buffer[*curr_pos] != EOF) {
        while(isspace(input_buffer[*curr_pos]))
            (*curr_pos)++;

        if(input_buffer[*curr_pos] == ';') {

            while(input_buffer[*curr_pos] != '\n')
                (*curr_pos)++;

            (*curr_pos)++;
            continue;
        }

        *inword_pos = *curr_pos;

        while(!isspace(input_buffer[*inword_pos]))
            (*inword_pos)++;

        input_buffer[*inword_pos] = '\0';

        DEBUG_PRINTF("input_buffer + curr_pos = %s\n", input_buffer + *curr_pos);
        //getchar();
        return;
    }
}

void ASM_dump(const char* code_data, uint64_t curr_code_byte) {

    for(int i = 0; i < curr_code_byte; i++) {

        if(i % 16 == 0) {

            DEBUG_PRINTF("\n");

            for(int j = i; j < i + 16; j++)
                DEBUG_PRINTF("%p ", code_data + j);

            DEBUG_PRINTF("\n");

        }
        DEBUG_PRINTF("      %02X ", code_data[i]);
    }
    DEBUG_PRINTF("\n");
    getchar();
}
void Labels_Ctor(Labels* labels, size_t labels_amount) {

    if(!labels) {

        DEBUG_PRINTF("ERROR: Null ptr\n");
        return;
    }

    labels->labels_amount = labels_amount;
    labels_data* tmp_ptr = (labels_data*) malloc(labels_amount*sizeof(labels_data));

    if(tmp_ptr) {

        labels->labels_arr = tmp_ptr;
        tmp_ptr = NULL;
    }

    else
        DEBUG_PRINTF("ERROR: memory was not allocated\n");
}
void Labels_Dtor(Labels* labels) {

    if(!labels || !labels->labels_arr) {

        DEBUG_PRINTF("ERROR: Null ptr\n");
        return;
    }

    free(labels->labels_arr);
}

int64_t Find_label(const char* label_name, Labels* labels) {

    for(int label_pos = 0; label_pos < labels->labels_amount; label_pos++) {

        if(!strcmp(label_name, labels->labels_arr[label_pos].label_name))
            return label_pos;

    }
    DEBUG_PRINTF("Label was not found\n");

    return -1;
}

void Label_handle(char* code_data, uint64_t* curr_code_byte, const char* code_str, Labels* labels) {

    int64_t label_pos = Find_label(code_str, labels);

    if(label_pos >= 0) {

        DEBUG_PRINTF("found\n");
        memcpy(&code_data[labels->labels_arr[label_pos].label_pos],
                curr_code_byte, sizeof(uint64_t));
        labels->labels_arr[label_pos].label_pos = *curr_code_byte;
        ASM_DUMP(code_data, *curr_code_byte);
    }

    else {

        DEBUG_PRINTF("not found\n");
        Insert_label(labels, code_str, *curr_code_byte);
        ASM_DUMP(code_data, *curr_code_byte);
    }
}

/*bool Check_jmp_func(const char* prev_cmd) {

    if(!strcmp(prev_cmd, "JA") || !strcmp(prev_cmd, "JAE") ||
       !strcmp(prev_cmd, "JB") || !strcmp(prev_cmd, "JBE") ||
       !strcmp(prev_cmd, "JE") || !strcmp(prev_cmd, "JNE") ||
       !strcmp(prev_cmd, "JMP") || !strcmp(prev_cmd, "CALL"))

        return true;

    return false;
}*/

void Insert_label(Labels* labels, const char* label_name, size_t label_pos) {

    labels->labels_arr[labels->labels_count].label_pos = label_pos;

    memcpy(labels->labels_arr[labels->labels_count].label_name,
        label_name, strlen(label_name) + 1);

    labels->labels_count++;
}

char Check_register(const char* code_str) {

    char* inword_pos = strstr(code_str, "X");

    if(inword_pos) {

        if(isalpha(*(inword_pos-1)))
            return *(inword_pos-1) - 'A' + 1;

        else
            return 'X' - 'A' + 1;
    }

    return -1;
}

bool Chech_brackets(const char* code_str) {

    if(code_str[0] == '[' && code_str[strlen(code_str)-1] == ']')
        return true;

    return false;

}

void ASM_Get_arg(char* code_data, uint64_t* curr_code_byte, const char* code_str, Labels* labels) {

    DEBUG_PRINTF("\n\n ASM_Get_arg\n");

    char* tmp_ptr = strchr(code_str, '+');

    if(tmp_ptr) {

        int tmp_const = NUMBER_MASK + REGISTER_MASK;

        DEBUG_PRINTF("+ \n");
        DEBUG_PRINTF("\n\ntmp_ptr = %s %s %s\n\n", tmp_ptr, tmp_ptr+1,tmp_ptr+2);

        code_data[*curr_code_byte+1] = Check_register(code_str);

        if(Chech_brackets(code_str)) {

            int64_t tmp_int = atoll(tmp_ptr+1);
            DEBUG_PRINTF("number + reg = %lld\n", tmp_int);
            memcpy(&code_data[*curr_code_byte+2],  &tmp_int, sizeof(int64_t));
            tmp_const += RAM_MASK;
            code_data[*curr_code_byte] = tmp_const;
            *curr_code_byte += 2*sizeof(char) + sizeof(double);
            DEBUG_PRINTF("[]\n");
        }
        else {

            double tmp_double = atof(tmp_ptr+1);
            memcpy(&code_data[*curr_code_byte+2],  &tmp_double, sizeof(double));
            code_data[*curr_code_byte] = tmp_const;
            *curr_code_byte += 2*sizeof(char) + sizeof(int64_t);
        }
    }

    else{

        if(Chech_brackets(code_str)) {

            code_data[*curr_code_byte] = RAM_MASK;
            DEBUG_PRINTF("[]\n");
            int push_register = Check_register(code_str);

            if(push_register >= 0) {

                DEBUG_PRINTF("%c%c\n", code_str[1], code_str[2]);
                code_data[*curr_code_byte] += REGISTER_MASK;
                code_data[*curr_code_byte+1] = push_register;
                *curr_code_byte += 2*sizeof(char);
            }
            else {

                double tmp_double = atof(tmp_ptr+1);
                DEBUG_PRINTF("number = %lg\n", tmp_double);
                code_data[*curr_code_byte] += NUMBER_MASK;
                memcpy(&code_data[*curr_code_byte+1],  &tmp_double, sizeof(double));
                *curr_code_byte += sizeof(char) + sizeof(double);
            }
        }

        else{

            int push_register = Check_register(code_str);

            if(push_register >= 0) {

                DEBUG_PRINTF("%c%c\n", code_str[0], code_str[1]);
                code_data[*curr_code_byte] = REGISTER_MASK;
                code_data[*curr_code_byte+1] = push_register;
                *curr_code_byte += 2*sizeof(char);
            }

            else {
                double tmp_double = 0;

                if(!strcmp(code_str, "NAN"))
                    tmp_double = NAN;

                else if(!strcmp(code_str, "INF"))
                    tmp_double = INFINITY;

                else
                    tmp_double = atof(code_str);

                DEBUG_PRINTF("number = %lg\n", tmp_double);

                code_data[*curr_code_byte] = NUMBER_MASK;
                memcpy(&code_data[*curr_code_byte+1], &tmp_double, sizeof(double));
                *curr_code_byte += sizeof(char) + sizeof(double);
            }
        }
    }
}

void ASM_Get_label(char* code_data, uint64_t* curr_code_byte, const char* code_str, Labels* labels) {

    DEBUG_PRINTF("ASM_Get_label: %s\n", code_str);
    int64_t label_pos = Find_label(code_str, labels);
    if(label_pos >= 0) {

        memcpy(&code_data[*curr_code_byte], &labels->labels_arr[label_pos].label_pos, sizeof(uint64_t));
        *curr_code_byte += sizeof(uint64_t);
    }

    else {

        Insert_label(labels, code_str, *curr_code_byte);
        *curr_code_byte += sizeof(uint64_t);
    }
}

void ASM_Void(char* code_data, uint64_t* curr_code_byte, const char* code_str, Labels* labels) {}
