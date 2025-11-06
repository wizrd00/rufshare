CC := tcc
CFLAGS := -std=c99 -g -c -Wc,-Werror=implicit-function-declaration,-Werror=implicit-int,-Werror=missing-prototypes,-Werror=pointer-sign,-Werror=sign-compare,-Werror=strict-prototypes,-Werror=shadow,-Werror=truncate,-Werror=unreachable-code
RUFSHARE_OBJ := ../../source/rufshare.c


test.elf : $(RUFSHARE_OBJ)
	@echo linking object files $^
	$(CC) $(INCLUDE_UNITY_LIB) -o $@ $^

$(TEST_OBJ) : $(TEST_SRC)
	@echo compiling $^
	$(CC) $(CFLAGS) -I$(FAKE_DIR) -I$(UNITY_DIR) -I$(INCLUDE_DIR) -o $@ $^

$(FAKE_OBJ) : $(FAKE_SRC) $(FAKE_HDR)
	@echo compiling $<
	$(CC) $(CFLAGS) -I$(FAKE_DIR) -I$(INCLUDE_DIR) -o $@ $<

memcheck : test.elf
	@echo start checking test.elf for any memory bug
	valgrind --tool=memcheck --leak-check=full --show-leak-kinds=all ./$^

clean :
	rm -rf $(TEST_OBJ) $(DATA_OBJ) $(MFILE_OBJ) $(NET_STREAM_OBJ) $(FILE_STREAM_OBJ) $(FAKE_OBJ) test.elf DEMOFILE*
