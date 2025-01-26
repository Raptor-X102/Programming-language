#include <stddef.h>
#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <unistd.h>

int64_t get_file_size(const char* filename);

int main() {

    int64_t file_size = get_file_size("tab.txt");
    FILE* file = fopen("tab.txt", "rb");
    char buffer[100] = {};
    fread(buffer, sizeof(char), file_size, file);
    for(int i = 0; buffer[i]; i++) {

        printf("(%d) ", i);
        printf("%%d = %d ", buffer[i]);
        printf("%%c = %c\n", buffer[i]);
    }

    printf("\nEnd\n");
}

int64_t get_file_size(const char* filename){

    int descriptor = open( filename, O_RDONLY);

    if (descriptor != -1) {
        FILE *file = fdopen(descriptor, "rb");
        struct stat statistics = {};
        if (file) {

            fstat(descriptor, &statistics);
            fclose(file);
        }
        close(descriptor);
        return statistics.st_size;
    }
    //DEBUG_PRINTF("In function 'Get_File_Size'\nERROR: file was not read\n");
    return 0;

}
