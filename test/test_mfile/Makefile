CC := pcc
CFLAGS := -std=c99 -O3 -g -Wc,-Werror=implicit-function-declaration,-Werror=implicit-int,-Werror=missing-prototypes,-Werror=pointer-sign,-Werror=sign-compare,-Werror=strict-prototypes,-Werror=shadow,-Werror=truncate,-Werror=unreachable-code
UNITY_LIB_FLAGS := -Wl,--library-path=../libunity,--library=unity,-rpath=../libunity

BUILD_DIR := build
INCLUDE_DIR := ../../include
UNITY_DIR := ../libunity

TEST_SRC := test.c
TEST_OBJ := $(BUILD_DIR)/test.o

MFILE_SRC := ../../source/mfile.c
MFILE_HDR := ../../include/mfile.h
MFILE_OBJ := $(BUILD_DIR)/mfile.o

TYPES_HDR := ../../include/types.h

test.elf : $(TEST_OBJ) $(MFILE_OBJ)
	@echo linking object files $^
	$(CC) $(CFLAGS) $(UNITY_LIB_FLAGS) -o $@ $^

$(TEST_OBJ) : $(TEST_SRC)
	@echo compiling $^
	$(CC) -c $(CFLAGS) -I$(INCLUDE_DIR) -I$(UNITY_DIR) -o $@ $^

$(MFILE_OBJ) : $(MFILE_SRC) $(MFILE_HDR) $(TYPES_HDR)
	@echo compiling $<
	$(CC) -c $(CFLAGS) -I$(INCLUDE_DIR) -o $@ $<

memcheck : test.elf
	@echo start checking test.elf for any memory bug
	valgrind --tool=memcheck --leak-check=full --show-leak-kinds=all ./$^

clean :
	rm -rf build/* test.elf DEMOFILE*
