CC := pcc
CFLAGS := -std=c99 -g -c -Wall -Werror -Wunsupported -Wimplicit-function-declaration
BUILD_DIR := build
INCLUDE_DIR := ../../include
UNITY_DIR := ../libunity
INCLUDE_UNITY_LIB := -Wl,--library-path=../libunity -Wl,--library=unity -Wl,-rpath=../libunity
TEST_SRC := test.c
TEST_OBJ := $(BUILD_DIR)/test.o
MFILE_SRC := ../../source/mfile.c
MFILE_HDR := ../../include/mfile.h
MFILE_OBJ := $(BUILD_DIR)/mfile.o
TYPES_HDR := ../../include/types.h

test.elf : $(TEST_OBJ) $(MFILE_OBJ)
	@echo linking object files $^
	$(CC) $(INCLUDE_UNITY_LIB) -o $@ $^

$(TEST_OBJ) : $(TEST_SRC)
	@echo compiling $^
	$(CC) $(CFLAGS) -I$(INCLUDE_DIR) -I$(UNITY_DIR) -o $@ $^

$(MFILE_OBJ) : $(MFILE_SRC) $(MFILE_HDR) $(TYPES_HDR)
	@echo compiling $<
	$(CC) $(CFLAGS) -I$(INCLUDE_DIR) -o $@ $<

memcheck : test.elf
	@echo start checking test.elf for any memory bug
	valgrind --tool=memcheck --leak-check=full --show-leak-kinds=all ./$^

clean :
	rm -rf $(TEST_OBJ) $(MFILE_OBJ) test.elf DEMOFILE*
