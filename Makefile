# Makefile command (for GCC): 'mingw32-make' 
CC = g++

INCLUDE_DIR = include
LIB_DIR = lib

SRC_DIR = src
OBJ_DIR = obj
$(shell mkdir $(OBJ_DIR)) # make '/obj' if non-existent
UTILS_DIR = utils

GLFLAGS = -lopengl32 -lglfw3 -lgdi32
CFLAGS = -O2

TARGET = main.exe

SOURCES := $(wildcard $(SRC_DIR)/*.c* $(UTILS_DIR)/*.c*) 
OBJECTS := $(patsubst $(SRC_DIR)/%.c, $(OBJ_DIR)/%.o, $(SOURCES)) #separately renaming .c and .cpp files
OBJECTS := $(patsubst $(SRC_DIR)/%.cpp, $(OBJ_DIR)/%.o, $(OBJECTS))
OBJECTS := $(patsubst $(UTILS_DIR)/%.cpp, $(OBJ_DIR)/%.o, $(OBJECTS))

test:
	echo $(SOURCES)

# Pattern rule for compiling files (%. $< $@)
$(OBJ_DIR)/%.o: $(SRC_DIR)/%.c
	$(CC) -c $< -o $@ -I$(INCLUDE_DIR)
	@echo Compiling $< to $@
 
$(OBJ_DIR)/%.o: $(SRC_DIR)/%.cpp
	$(CC) -c $< -o $@ -I$(INCLUDE_DIR)
	@echo Compiling $< to $@

$(OBJ_DIR)/%.o: $(UTILS_DIR)/%.cpp
	$(CC) -c $< -o $@ -I$(INCLUDE_DIR)
	@echo Compiling $< to $@

$(TARGET): $(OBJECTS)
	$(CC) $^ -o $@ -I$(INCLUDE_DIR) -L$(LIB_DIR) $(GLFLAGS) $(CFLAGS)

#COMMANDS:

compile: $(TARGET)

run:
	./$(TARGET)

.PHONY: clean

clean:
	-if exist $(OBJ_DIR) del /f /q $(OBJ_DIR)\*.o 2>nul
#	del $(TARGET)
#	$(shell rm $(OBJ_DIR)/*)
#whatever Windows
	
all:
	mingw32-make compile
	mingw32-make run
#	mingw32-make clean

# $< first dep
# $^ all deps
# $@ function name