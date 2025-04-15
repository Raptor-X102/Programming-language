CC = g++

DEBUG = NO_DEBUG

CFLAGS = -Wformat=2 -Wshadow -Winit-self -Wredundant-decls -Wcast-align -Wundef -Wfloat-equal \
-Winline -Wunreachable-code -Wmissing-declarations -Wmissing-include-dirs \
-Wswitch-enum -Wswitch-default -Weffc++ -Wmain -Wextra -Wall -g -pipe -fexceptions \
-Wcast-qual -Wconversion -Wctor-dtor-privacy -Wempty-body -Wformat-security -Wformat=2 \
-Wignored-qualifiers -Wlogical-op -Wno-missing-field-initializers -Wnon-virtual-dtor \
-Woverloaded-virtual -Wpointer-arith -Wsign-promo -Wstack-usage=8192 -Wstrict-aliasing \
-Wstrict-null-sentinel -Wtype-limits -Wwrite-strings -Werror=vla -D_DEBUG -D_EJUDGE_CLIENT_SIDE

SOURCES_DIR = src
SOURCES = $(wildcard $(SOURCES_DIR)/*.cpp)
HEADERS_DIR = headers
GLOBAL_HEADERS = ..\headers
BIN_TREES = Binary_Trees
BIN_TREES_SRC = $(BIN_TREES)/src
BIN_TREES_HEADERS = $(BIN_TREES)/headers
LIB_BIN_TREES_SRC = $(wildcard $(BIN_TREES_SRC)/*.cpp)
LIBS_CHECK_R_W_FLAGS = Check_flags
LIB_CHECK_R_W_FLAGS_SRC = $(wildcard $(LIBS_CHECK_R_W_FLAGS)/*.cpp)
STACK = Binary_Trees\Stack
HEADERS_STACK = $(STACK)/headers
SRC_STACK = $(STACK)/src
LIB_STACK_SRC = $(wildcard $(SRC_STACK)/*.cpp)
LIBS_SPU = Processor\src
LIBS_SPU_HEADERS = Processor\headers
LIBS_SPU_SRC = $(wildcard $(LIBS_SPU)/*.cpp)
GET_FILE_SIZE = Binary_Trees\Get_file_size
GET_FILE_SIZE_HEADERS = $(GET_FILE_SIZE)
GET_FILE_SIZE_SRC = $(wildcard $(GET_FILE_SIZE)/*.cpp)

BUILD_DIR = build
OBJ_DIR = obj
OBJECTS = $(wildcard $(OBJ_DIR)/*.o)
EXECUTABLE = lang.exe

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

create_obj:
	$(CC) -D $(DEBUG) $(SOURCES) $(LIB_BIN_TREES_SRC) $(LIB_CHECK_R_W_FLAGS_SRC) $(GET_FILE_SIZE_SRC) $(LIB_STACK_SRC) $(LIBS_SPU_SRC) -c -I $(HEADERS_STACK) -I $(BIN_TREES_HEADERS) -I $(GET_FILE_SIZE_HEADERS) -I $(LIBS_SPU_HEADERS) -I $(HEADERS_DIR) -I $(LIBS_CHECK_R_W_FLAGS) -I $(LIBS_STACK) $(CFLAGS)
	mv *.o $(OBJ_DIR)

linkage:
	$(CC) -D $(DEBUG) $(OBJECTS) -o $(BUILD_DIR)/$(EXECUTABLE)

compile:
	make create_obj
	make linkage
