CC := gcc
OBJ_DIR := build
CFLAGS := -std=c99
INCLUDES := -I../include -Ilibunity
# add net_stream.c 
unity_src := ./libunity/unity.c
unity_obj := ./$(OBJ_DIR)/unity.o

test_init_tcp_socket_source := ./test_init_tcp_socket.c
test_init_tcp_socket_object := ./$(OBJ_DIR)/test_init_tcp_socket.o
test_init_tcp_socket_executable := ./test_init_tcp_socket.elf

$(unity_obj) : $(unity_src)
	$(CC) $(CFLAGS) -Ilibunity -c $(unity_src) -o $(unity_obj)

$(test_init_tcp_socket_object) : $(test_init_tcp_socket_source)
	$(CC) $(CFLAGS) $(INCLUDES) -c $(test_init_tcp_socket_source) -o $(test_init_tcp_socket_object)

$(test_init_tcp_socket_executable) : $(unity_obj)

test_init_tcp_socket : $(unity_obj) $(test_init_tcp_socket_object)
	$(CC) $(unity_obj) $(test_init_tcp_socket_object) -o $(test_init_tcp_socket_executable)

clean_test_init_tcp_socket :
	rm -f $(test_init_tcp_socket_object) $(test_init_tcp_socket_executable)

clean :
	rm -f $(wildcard $(OBJ_DIR)/*.o) $(wildcard ./*.elf)


