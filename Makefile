# source files.
FILES= $(wildcard ./src/*.c) 
OBJECTS =$(notdir $(FILES))
TARGETS= main
CC=gcc
ifeq ($(shell uname -s),Darwin)
    LIB=libMix.dylib
	SHARED=-dynamiclib -install_name ./../StreamMixLib/$(LIB)
else
	LIB=libMix.so
	SHARED=-shared
endif

OBJ_DIR=./objs/
SRC=./src/
TEST_DIR=./test/
CFLAGS+= -g
LIBS ?= -L./objs -lm -lfdk-aac
INCLUDES = -I./src/ 

all:$(TARGETS)

OBJECT = $(OBJECTS:%.c=%.o)
main: $(OBJECT) test.o
	@echo creating mixtest
	@$(CC) $(OBJECT:%=$(OBJ_DIR)%) $(OBJ_DIR)test.o -o $(OBJ_DIR)mixtest $(LIBS)

test.o:
	@echo compiling $(@:%.o=%.c)
	@$(CC) $(CFLAGS) $(INCLUDES) -c -o $(OBJ_DIR)$(@) $(TEST_DIR)$(@:%.o=%.c)

$(OBJECT): 
	@echo compiling $(@:%.o=%.c)
	@$(CC) $(CFLAGS) $(INCLUDES) -c -o $(OBJ_DIR)$(@) $(SRC)$(@:%.o=%.c)

clean:
	rm -f $(OBJECT) $(LIB)
	rm -f $(OBJ_DIR)*

#install:
#	cp -f $(LIB) /usr/lib
