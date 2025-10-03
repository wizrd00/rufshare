CC := tcc
CFLAGS := -std=c99 -g -c -Wall -Werror -Wimplicit-function-declaration
BUILD_DIR := build
FAKE_DIR := fake
INCLUDE_DIR := ../../include/
UNITY_DIR := ../libunity/
UNITY_SRC := ../libunity/unity.c
UNITY_OBJ := ../$(BUILD_DIR)/unity.o
TEST_SRC := test.c
TEST_OBJ := $(BUILD_DIR)/test.o
NET_STREAM_SRC := ../../source/net_stream.c
NET_STREAM_HDR := fake/net_stream.h
NET_STREAM_OBJ := $(BUILD_DIR)/net_stream.o
FAKE_SRC := fake/fake_socket.c
FAKE_HDR := fake/fake_socket.h
FAKE_OBJ := $(BUILD_DIR)/fake_socket.o
TYPES_HDR := ../../include/types.h

test.elf : $(UNITY_OBJ) $(TEST_OBJ) $(NET_STREAM_OBJ) $(FAKE_OBJ)
	@echo linking object files $^
	$(CC) -o $@ $^

$(UNITY_OBJ) : $(UNITY_SRC)
	@echo compiling $^
	$(CC) $(CFLAGS) -I$(UNITY_DIR) -o $@ $^

$(TEST_OBJ) : $(TEST_SRC)
	@echo compiling $^
	$(CC) $(CFLAGS) -I$(FAKE_DIR) -I$(UNITY_DIR) -I$(INCLUDE_DIR) -o $@ $^

$(NET_STREAM_OBJ) : $(NET_STREAM_SRC) $(NET_STREAM_HDR) $(TYPES_HDR)
	@echo compiling $<
	$(CC) $(CFLAGS) -I$(FAKE_DIR) -I$(INCLUDE_DIR) -o $@ $<

$(FAKE_OBJ) : $(FAKE_SRC) $(FAKE_HDR)
	@echo compiling $<
	$(CC) $(CFLAGS) -I$(FAKE_DIR) -o $@ $<

clean :
	rm -rf $(TEST_OBJ) $(NET_STREAM_OBJ) $(FAKE_OBJ) test.elf
