# source files.
OBJECTS = mix
TARGETS= test
CC=gcc
ifeq ($(shell uname -s),Darwin)
    LIB=libCumulus.dylib
	SHARED=-dynamiclib -install_name ./../CumulusLib/$(LIB)
else
	LIB=libMix.so
	SHARED=-shared
endif

OBJ_DIR=./objs/
SRC=./src/
TEST_DIR=./test/
CFLAGS+= -g
LIBS ?= -L./objs -lm
INCLUDES = -I./src/ 

all:$(TARGETS)

test: test.o mix.o
	@echo creating mixtest
	@$(CC) -o mixtest $(wildcard $(OBJ_DIR)*.o) $(LIBS)
test.o:
	@echo compiling $(@:%.o=%.c)
	@$(CC) $(CFLAGS) $(INCLUDES) -c -o $(OBJ_DIR)$(@) $(TEST_DIR)test.c

OBJECT = $(OBJECTS:%=%.o)
#main: $(OBJECT)
#	@echo creating dynamic lib $(LIB)
#	@$(CC) $(OBJECT:%=$(OBJ_DIR)%) -o $(LIB) $(LIBS)

$(OBJECT): 
	@echo compiling $(@:%.o=%.c)
	@$(CC) $(CFLAGS) $(INCLUDES) -c -o $(OBJ_DIR)$(@) $(SRC)$(@:%.o=%.c)

clean:
	rm -f $(OBJECT) $(LIB)
	rm -f $(OBJ_DIR)*

#install:
#	cp -f $(LIB) /usr/lib
