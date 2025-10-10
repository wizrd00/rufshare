CC := tcc
CFLAGS := -std=c99 -g -c -Wall -Werror -Wunsupported -Wimplicit-function-declaration
BUILD_DIR := build
INCLUDE_DIR := ../../include
UNITY_DIR := ../libunity
INCLUDE_UNITY_LIB := -L../libunity -lunity -Wl,-rpath=../libunity
TEST_SRC := test.c
TEST_OBJ := $(BUILD_DIR)/test.o
FILE_STREAM_SRC := ../../source/file_stream.c
FILE_STREAM_HDR := ../../include/file_stream.h
FILE_STREAM_OBJ := $(BUILD_DIR)/file_stream.o
MFILE_SRC := ../../source/mfile.c
MFILE_HDR := ../../include/mfile.h
MFILE_OBJ := $(BUILD_DIR)/mfile.o
TYPES_HDR := ../../include/types.h

test.elf : $(TEST_OBJ) $(FILE_STREAM_OBJ) $(MFILE_OBJ)
	@echo linking object files $^
	$(CC) $(INCLUDE_UNITY_LIB) -o $@ $^

$(TEST_OBJ) : $(TEST_SRC)
	@echo compiling $^
	$(CC) $(CFLAGS) -I$(INCLUDE_DIR) -I$(UNITY_DIR) -o $@ $^

$(FILE_STREAM_OBJ) : $(FILE_STREAM_SRC) $(FILE_STREAM_HDR) $(TYPES_HDR)
	@echo compiling $<
	$(CC) $(CFLAGS) -I$(INCLUDE_DIR) -o $@ $<

$(MFILE_OBJ) : $(MFILE_SRC) $(MFILE_HDR) $(TYPES_HDR)
	@echo compiling $<
	$(CC) $(CFLAGS) -I$(INCLUDE_DIR) -o $@ $<

memcheck : test.elf
	@echo start checking test.elf for any memory bug
	valgrind --tool=memcheck ./$^

clean :
	rm -rf $(TEST_OBJ) $(FILE_STREAM_OBJ) $(MFILE_OBJ) test.elf DEMOFILE
