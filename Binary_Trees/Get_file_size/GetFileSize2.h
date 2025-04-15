#ifndef GETFILESIZE2
#define GETFILESIZE2
#include <stddef.h>
#include <stdio.h>
#include <stdlib.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <unistd.h>
#include "DEBUG_PRINTF.h"

size_t get_file_size(const char* filename);

#endif
