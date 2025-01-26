#include "Assembler.h"

void Compile_code(const char* compiling_file, const char* out_file, Labels* labels) {

    uint64_t file_size = get_file_size(compiling_file); DEBUG_PRINTF("Size was gotten\n");

    FILE* input_file = fopen(compiling_file, "rb");
    if(!input_file) {

        DEBUG_PRINTF("ERROR: file was not opened");
        return;
    }

    uint64_t commands_amount = 0;
    //char* output_file = Compiled_filename(compiling_file);
    FILE* output_file = fopen(out_file, "wb+");
    if(!output_file) {

        DEBUG_PRINTF("ERROR: file was not opened");
        return;
    }

    DEBUG_PRINTF("convertion started\n");
    commands_amount = Convert_txt_to_code(input_file, output_file, file_size, labels);
    DEBUG_PRINTF("commands_amount = %d", commands_amount);
    DEBUG_PRINTF("ended\n");

    fclose(output_file);
    DEBUG_PRINTF("input file was closed\n");

    //free(output_file);
    DEBUG_PRINTF("memory was free\n");

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

    uint64_t commands_amount = 0;
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


    char signature[signature_len] = {};
    int version = 0;

    fread(signature, sizeof(char), 5, input_file);
    DEBUG_PRINTF("%s\n", signature);
    fwrite(signature, sizeof(char), 5, output_file);
    fseek(input_file, 2L, SEEK_CUR);
    fscanf(input_file, "%d", &version);
    fwrite(&version, sizeof(int), 1, output_file);
    fseek(input_file, 10L, SEEK_SET);
    DEBUG_PRINTF("%d\n", version);

    fread(input_buffer, sizeof(char), code_length-7*sizeof(char)-1*sizeof(int), input_file);
    char* code_str_prev = input_buffer;
    uint64_t curr_code_byte = 0;
    for(uint64_t curr_pos = 0, inword_pos = 0; input_buffer[curr_pos] != EOF &&
        curr_pos < code_length - 7 * sizeof(char) - 1 * sizeof(int); curr_pos++) { // fixme: scanf
        DEBUG_PRINTF("\n\n");
        while(isspace(input_buffer[curr_pos]))
            curr_pos++;

        if(input_buffer[curr_pos] == ';') {

            while(input_buffer[curr_pos] != '\n')
                curr_pos++;

            continue;
        }
        inword_pos = curr_pos;

        while(!isspace(input_buffer[inword_pos]))
            inword_pos++;

        input_buffer[inword_pos] = '\0';
        DEBUG_PRINTF("input_buffer + curr_pos = %s\n", input_buffer + curr_pos);

        for(int i = 0; i < sizeof(Commands) / sizeof(Commands[0]); i++) {

            if(!strcmp(input_buffer+curr_pos, Commands[i].command_name)) {

                code_data[curr_code_byte] = (char) Commands[i].command_num;
                curr_code_byte++;
                goto end_loop;
            }
        }

        if(strchr(input_buffer+curr_pos, ':')) {

            if(!Check_jmp_func(code_str_prev)) {
                int64_t label_pos = Find_label(input_buffer+curr_pos, labels);
                if(label_pos >= 0) {

                    memcpy(&code_data[labels->labels_arr[label_pos].label_pos], &curr_code_byte, sizeof(uint64_t));
                    code_str_prev = input_buffer+curr_pos;
                    DEBUG_PRINTF("code_str_prev = %s\n", code_str_prev);
                    curr_pos = inword_pos;
                    ASM_DUMP(code_data, curr_code_byte);
                    continue;
                }
                else {

                    Insert_label(labels, input_buffer+curr_pos, curr_code_byte);
                    code_str_prev = input_buffer+curr_pos;
                    DEBUG_PRINTF("code_str_prev = %s\n", code_str_prev);
                    curr_pos = inword_pos;
                    ASM_DUMP(code_data, curr_code_byte);
                    continue;
                }
            }
            else {

                int64_t label_pos = Find_label(input_buffer+curr_pos, labels);
                if(label_pos >= 0) {

                    memcpy(&code_data[curr_code_byte], &labels->labels_arr[label_pos].label_pos, sizeof(uint64_t));
                    curr_code_byte += sizeof(uint64_t);
                }
                else {

                    Insert_label(labels, input_buffer+curr_pos, curr_code_byte);
                    curr_code_byte += sizeof(uint64_t);

                }
            }
        }
        else if(!strcmp(code_str_prev, "PUSH") || !strcmp(code_str_prev, "POP")) {

            DEBUG_PRINTF("PUSH params: \n");
            ASM_Get_arg(code_data, &commands_amount, &curr_code_byte, input_buffer+curr_pos);
        }

        else{
            int64_t tmp_int = atoll(input_buffer+curr_pos);
            memcpy(&code_data[curr_code_byte], &tmp_int, sizeof(int64_t));
            curr_code_byte += sizeof(int64_t);
            DEBUG_PRINTF("    [%d]atoi = %d\n", commands_amount, tmp_int);
        }

        end_loop:
            ASM_DUMP(code_data, curr_code_byte);
            code_str_prev = input_buffer+curr_pos;
            DEBUG_PRINTF("code_str_prev = %s\n", code_str_prev);
            curr_pos = inword_pos;
            commands_amount++;

    }

    //txDump(code_data);
    fwrite(&curr_code_byte, sizeof(uint64_t), 1, output_file);
    fwrite(code_data, sizeof(char), curr_code_byte, output_file);

    return curr_code_byte;
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

bool Check_jmp_func(const char* prev_cmd) {

    if(!strcmp(prev_cmd, "JA") || !strcmp(prev_cmd, "JAE") || !strcmp(prev_cmd, "JB") || !strcmp(prev_cmd, "JBE") ||
       !strcmp(prev_cmd, "JE") || !strcmp(prev_cmd, "JNE") || !strcmp(prev_cmd, "JMP") || !strcmp(prev_cmd, "CALL"))
        return true;
    return false;

}

void Insert_label(Labels* labels, const char* label_name, size_t label_pos) {

    labels->labels_arr[labels->labels_count].label_pos = label_pos;
    memcpy(labels->labels_arr[labels->labels_count].label_name, label_name, strlen(label_name) + 1);
    labels->labels_count++;

}

char Check_register(const char* code_str) {

    char* inword_pos = strstr(code_str, "X");
    if(inword_pos) {

        if(isalpha(*(inword_pos-1)))
            return *(inword_pos-1) - 'A' + 1;

        else {

            return 'X' - 'A' + 1;

        }
    }

    return -1;
}

bool Chech_brackets(const char* code_str) {

    if(code_str[0] == '[' && code_str[strlen(code_str)-1] == ']')
        return true;

    return false;

}

void ASM_Get_arg(char* code_data, uint64_t* commands_amount, uint64_t* curr_code_byte, const char* code_str) {

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

        *commands_amount += 2;

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
                (*commands_amount)++;
            }
            else {

                double tmp_double = atof(tmp_ptr+1);
                DEBUG_PRINTF("number = %lg\n", tmp_double);
                code_data[*curr_code_byte] += NUMBER_MASK;
                memcpy(&code_data[*curr_code_byte+1],  &tmp_double, sizeof(double));
                *curr_code_byte += sizeof(char) + sizeof(double);
                (*commands_amount)++;

            }
        }
        else{

            int push_register = Check_register(code_str);
            if(push_register >= 0) {

                DEBUG_PRINTF("%c%c\n", code_str[0], code_str[1]);
                code_data[*curr_code_byte] = REGISTER_MASK;
                code_data[*curr_code_byte+1] = push_register;
                *curr_code_byte += 2*sizeof(char);
                (*commands_amount)++;
            }
            else {
                double tmp_double = atof(code_str);
                DEBUG_PRINTF("number = %lg\n", tmp_double);
                DEBUG_PRINTF("code_data + *commands_amount (%d) = %p\n", *commands_amount, code_data + *curr_code_byte);
                code_data[*curr_code_byte] = NUMBER_MASK;
                memcpy(&code_data[*curr_code_byte+1], &tmp_double, sizeof(double));
                *curr_code_byte += sizeof(char) + sizeof(double);
                (*commands_amount)++;
            }
        }
    }
}

void Get_draw_arg(char* input_buffer, uint64_t* height, uint64_t* width, uint64_t* inword_pos) {

    *height = atoll(input_buffer);
    uint64_t curr_byte = *inword_pos;
    (*inword_pos)++;
    while(!isspace(input_buffer[*inword_pos]))
            (*inword_pos)++;

    input_buffer[*inword_pos] = '\0';

    *width = atoll(input_buffer + curr_byte + 1);
}
