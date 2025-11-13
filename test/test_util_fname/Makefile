CC := pcc
CFLAGS := -std=c99 -O3 -g -Wc,-Werror=implicit-function-declaration,-Werror=implicit-int,-Werror=missing-prototypes,-Werror=pointer-sign,-Werror=sign-compare,-Werror=strict-prototypes,-Werror=shadow,-Werror=truncate
UNITY_LIB_FLAGS := -Wl,--library-path=../libunity,--library=unity,-rpath=../libunity

BUILD_DIR := build
INCLUDE_DIR := ../../include
UNITY_DIR := ../libunity

TEST_SRC := test.c
TEST_OBJ := $(BUILD_DIR)/test.o

UTIL_FNAME_HDR := ../../include/utils/fname.h

TYPES_HDR := ../../include/types.h

test.elf : $(TEST_OBJ) $(UTIL_FNAME_HDR)
	@echo linking object files $^
	$(CC) $(CFLAGS) $(UNITY_LIB_FLAGS) -o $@ $<

$(TEST_OBJ) : $(TEST_SRC)
	@echo compiling $^
	$(CC) -c $(CFLAGS) -I$(INCLUDE_DIR) -I$(UNITY_DIR) -o $@ $^

memcheck : test.elf
	@echo start checking test.elf for any memory bug
	valgrind --tool=memcheck --leak-check=full --show-leak-kinds=all ./$^

clean :
	rm -rf build/* test.elf DEMOFILE*
