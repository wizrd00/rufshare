CC := pcc
CFLAGS := -std=c99 -g -Wc,-Werror=implicit-function-declaration,-Werror=implicit-int,-Werror=missing-prototypes,-Werror=pointer-sign,-Werror=sign-compare,-Werror=strict-prototypes,-Werror=shadow,-Werror=truncate
UNITY_LIB_FLAGS := -Wl,--library-path=../libunity,--library=unity,-rpath=../libunity

BUILD_DIR := build
FAKE_DIR := fake
INCLUDE_DIR := ../../include/
UNITY_DIR := ../libunity

TEST_SRC := test.c
TEST_OBJ := $(BUILD_DIR)/test.o

NET_STREAM_SRC := ../../source/net_stream.c
NET_STREAM_HDR := fake/net_stream.h
NET_STREAM_OBJ := $(BUILD_DIR)/net_stream.o

FAKE_SRC := fake/fake_socket.c
FAKE_HDR := fake/fake_socket.h
FAKE_OBJ := $(BUILD_DIR)/fake_socket.o

TYPES_HDR := ../../include/types.h

test.elf : $(TEST_OBJ) $(NET_STREAM_OBJ) $(FAKE_OBJ)
	@echo linking object files $^
	$(CC) $(CFLAGS) $(UNITY_LIB_FLAGS) -o $@ $^

$(TEST_OBJ) : $(TEST_SRC)
	@echo compiling $^
	$(CC) -c $(CFLAGS) -I$(FAKE_DIR) -I$(UNITY_DIR) -I$(INCLUDE_DIR) -o $@ $^

$(NET_STREAM_OBJ) : $(NET_STREAM_SRC) $(NET_STREAM_HDR) $(TYPES_HDR)
	@echo compiling $<
	$(CC) -c $(CFLAGS) -I$(FAKE_DIR) -I$(INCLUDE_DIR) -o $@ $<

$(FAKE_OBJ) : $(FAKE_SRC) $(FAKE_HDR)
	@echo compiling $<
	$(CC) -c $(CFLAGS) -I$(FAKE_DIR) -I$(INCLUDE_DIR) -o $@ $<

memcheck : test.elf
	@echo start checking test.elf for any memory bug
	valgrind --tool=memcheck --leak-check=full --show-leak-kinds=all ./$^

clean :
	rm -rf build/* test.elf
