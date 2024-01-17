TARGET_EXEC ?= cchat
CC = clang

BIN_DIR ?= ./bin
BUILD_DIR ?= ./build
SRC_DIRS ?= ./src
LIB_DIR ?= 

SRCS := $(shell find $(SRC_DIRS) -name *.c)
OBJS := $(SRCS:%=$(BUILD_DIR)/%.o)
DEPS := $(OBJS:.o=.d)

INC_DIRS := $(shell find ./src -type d -not -path "./*.git*")
INC_FLAGS := $(addprefix -I,$(INC_DIRS))

CFLAGS ?= $(INC_FLAGS) -Wall -O2 -D_FORTIFY_SOURCE=1 -fstack-protector	\
	  -Wformat-security -fPIE -Wextra -Wpedantic -Wcast-qual 	\
	  -Walloca -Wnull-dereference -Wstack-protector -Wvla 		\
	  -Wconditional-uninitialized -Wloop-analysis -Wfloat-equal 	\
	  -Wbad-function-cast -Wpointer-arith -Widiomatic-parentheses 	\
	  -Wthread-safety -fstack-protector-strong 

LDFLAGS ?= $(addprefix -L,$(LIB_DIR)) -lpthread -lssl -lcrypto -lconfig \
	   -Wl,--strip-all  -pie -Wl,-z,relro -fsanitize=safe-stack 	\
	   -fsanitize=cfi -flto 

$(BIN_DIR)/$(TARGET_EXEC): $(OBJS)
	$(CC) $(OBJS) -o $@ $(LDFLAGS)

$(BUILD_DIR)/%.c.o: %.c
	$(MKDIR_P) $(dir $@)
	$(CC) $(CFLAGS) -c $< -o $@

.PHONY: clean

clean:
	$(RM) $(BIN_DIR)/$(TARGET_EXEC)
	$(RM) -r $(BUILD_DIR)

-include $(DEPS)

MKDIR_P ?= mkdir -p
