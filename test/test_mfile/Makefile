CC := tcc
CFLAGS := -std=c99 -g -c -Wall -Werror -Wimplicit-function-declaration
BUILD_DIR := build
UNITY_SRC := ../libunity/unity.c
UNITY_OBJ := ../$(BUILD_DIR)/unity.o
TEST_SRC := test.c
TEST_OBJ := $(BUILD_DIR)/test.o
MFILE_SRC := ../../source/mfile.c
MFILE_HDR := ../../include/mfile.h
MFILE_OBJ := $(BUILD_DIR)/mfile.o
TYPES_HDR := ../../include/types.h

test.elf : $(UNITY_OBJ) $(TEST_OBJ) $(MFILE_OBJ)
	@echo linking object files $^
	$(CC) -o $@ $^

$(UNITY_OBJ) : $(UNITY_SRC)
	@echo compiling $^
	$(CC) $(CFLAGS) -I../libunity -o $@ $^

$(TEST_OBJ) : $(TEST_SRC)
	@echo compiling $^
	$(CC) $(CFLAGS) -I../../include -I../libunity -o $@ $^

$(MFILE_OBJ) : $(MFILE_SRC) $(MFILE_HDR) $(TYPES_HDR)
	@echo compiling $<
	$(CC) $(CFLAGS) -I../../include -o $@ $<

clean :
	rm -rf $(TEST_OBJ) $(MFILE_OBJ) test.elf DEMOFILE*
