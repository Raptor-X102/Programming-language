CC = g++

CFLAGS = -D NO_DEBUG -Wformat=2 -Wshadow -Winit-self -Wredundant-decls -Wcast-align -Wundef -Wfloat-equal \
-Winline -Wunreachable-code -Wmissing-declarations -Wmissing-include-dirs \
-Wswitch-enum -Wswitch-default -Weffc++ -Wmain -Wextra -Wall -g -pipe -fexceptions \
-Wcast-qual -Wconversion -Wctor-dtor-privacy -Wempty-body -Wformat-security -Wformat=2 \
-Wignored-qualifiers -Wlogical-op -Wno-missing-field-initializers -Wnon-virtual-dtor \
-Woverloaded-virtual -Wpointer-arith -Wsign-promo -Wstack-usage=8192 -Wstrict-aliasing \
-Wstrict-null-sentinel -Wtype-limits -Wwrite-strings -Werror=vla -D_DEBUG -D_EJUDGE_CLIENT_SIDE

DEBUG = NO_DEBUG
SOURCES_DIR = src
ASM = ASM
SPU = SPU
SOURCES = $(wildcard $(SOURCES_DIR)/*.cpp)
HEADERS_DIR = headers
GLOBAL_HEADERS = ..\headers
STACK = Stack
HEADERS_STACK = $(STACK)/headers
SRC_STACK = $(STACK)/src
SRC_STACK_CPP = $(wildcard $(SRC_STACK)/*.cpp)
GET_FILE_SIZE = Get_file_size
GET_FILE_SIZE_HEADERS = $(GET_FILE_SIZE)
GET_FILE_SIZE_SRC = $(wildcard Get_file_size/*.cpp)
CHECK_FLAGS = Check_flags
HEADERS_CHECK_FLAGS = $(CHECK_FLAGS)
SRC_CHECK_FLAGS = $(wildcard $(CHECK_FLAGS)/*.cpp)

BUILD_DIR = build
MAIN_DIR = main
OBJ_DIR = obj
OBJECTS = $(wildcard $(OBJ_DIR)/*.o)
EXECUTABLE_SPU = spu.exe
EXECUTABLE_ASM = asm.exe

setup:
	mkdir $(BUILD_DIR)
	mkdir $(OBJ_DIR)
	mkdir $(SOURCES_DIR)
	mkdir $(HEADERS_DIR)

clean_exe:
	rm $(BUILD_DIR)/*.exe

clean_obj:
	rm $(OBJ_DIR)/*.o

clean_dump:
	rm dump/*.png
	rm dump/*.dot

clean:
	make clean_obj

create_obj_asm:
	$(CC) $(SOURCES) $(MAIN_DIR)/Assembler_main.cpp $(GET_FILE_SIZE_SRC) $(SRC_CHECK_FLAGS) $(SRC_STACK_CPP) -c -I $(HEADERS_DIR) -I $(HEADERS_STACK) -I $(GET_FILE_SIZE_HEADERS) -I $(HEADERS_CHECK_FLAGS) $(CFLAGS)
	mv *.o $(OBJ_DIR)

linkage_asm:
	$(CC) $(OBJECTS) -o $(BUILD_DIR)/$(EXECUTABLE_ASM)

compile_asm:
	make create_obj_asm
	make linkage_asm

create_obj_spu:
	$(CC) $(SOURCES) $(MAIN_DIR)/Processor_main.cpp $(GET_FILE_SIZE_SRC) $(SRC_CHECK_FLAGS) $(SRC_STACK_CPP) -c -I $(HEADERS_DIR) -I $(HEADERS_CHECK_FLAGS) -I $(GET_FILE_SIZE_HEADERS) -I $(HEADERS_STACK) $(CFLAGS)
	mv *.o $(OBJ_DIR)

linkage_spu:
	$(CC) $(OBJECTS) -o $(BUILD_DIR)/$(EXECUTABLE_SPU)

compile_spu:
	make create_obj_spu
	make linkage_spu
