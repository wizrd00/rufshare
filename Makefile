CC ?= pcc
ifeq ($(CC), pcc)
	CFLAGS := -std=c99 -O3 -Wc,-Werror=implicit-function-declaration,-Werror=missing-prototypes,-Werror=pointer-sign,-Werror=sign-compare,-Werror=strict-prototypes,-Werror=shadow -pthread
	CFLAGS_PIC := -shared -fPIC 
	CFLAGS_LOG := -D LOG_TRACE -D LOG_DEBUG -D LOG_ERROR

else ifeq ($(CC), gcc)
	CFLAGS := -std=c99 -O3 -Wall -Wextra -Wpedantic -Wstrict-aliasing -Wcast-align -Wconversion -Wsign-conversion -Wshadow -Wswitch-enum -pthread
	CFLAGS_PIC := -shared -fPIC
	CFLAGS_LOG := -D LOG_TRACE -D LOG_ERROR
else
	$(error unsupported compiler : $(CC))
endif
LIB_FLAGS := -Wl,--library-path=$(LIB_DIR),-rpath=$(LIB_DIR)
SRC_DIR := source
INC_DIR := include
BIN_DIR := binary
LIB_DIR := library

SRC_FILES := $(wildcard $(SRC_DIR)/*.c) $(wildcard $(SRC_DIR)/logging/*.c)
HDR_FILES := $(wildcard $(INC_DIR)/*.h) $(wildcard $(INC_DIR)/logging/*.h)
OBJ_FILES := $(patsubst $(SRC_DIR)/%.c, $(BIN_DIR)/%.o, $(SRC_FILES))

INCLUDE_FLAGS := -I$(INC_DIR)

LIBRUFSHARE := $(LIB_DIR)/librufshare.so
LIBCRC := $(LIB_DIR)/libcrc.a

POINTER_SYM := "\e[91m->\e[0m"

$(LIBRUFSHARE) : $(BIN_DIR) $(OBJ_FILES) $(LIBCRC) $(HDR_FILES)
	@/usr/bin/echo -e $(POINTER_SYM) "\e[96mlinking modules into" $@ "\e[0m"
	$(CC) $(CFLAGS) $(CFLAGS_PIC) $(CFLAGS_LOG) -o $@ $(OBJ_FILES) $(LIBCRC)
	@/usr/bin/echo -e $(POINTER_SYM) "\e[93mstrip" $@ "\e[0m"
	@strip $@
	@./cpy.sh

$(BIN_DIR) :
	@mkdir -p $(BIN_DIR)
	@mkdir -p $(BIN_DIR)/logging

$(BIN_DIR)/%.o : $(SRC_DIR)/%.c $(INC_DIR)/%.h
	@/usr/bin/echo -e $(POINTER_SYM) "\e[93mcompiling module" $< "\e[0m"
	$(CC) -c $(CFLAGS) $(CFLAGS_PIC) $(CFLAGS_LOG) $(INCLUDE_FLAGS) -o $@ $<

clean :
	rm $(wildcard $(BIN_DIR)/*.o) $(wildcard $(BIN_DIR)/logging/*.o) $(wildcard $(LIBRUFSHARE))

clear : clean
