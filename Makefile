TARGET_EXEC ?= cchat
CC ?= 

BIN_DIR ?= ./bin
BUILD_DIR ?= ./build
SRC_DIRS ?= ./src
LIB_DIR ?= 

SRCS != find $(SRC_DIRS) -name *.c
OBJS := $(SRCS:%=$(BUILD_DIR)/%.o)
DEPS := $(OBJS:.o=.d)

INC_DIRS != find ./src -type d -not -path "./*.git*"
INC_FLAGS := $(addprefix -I,$(INC_DIRS))

CFLAGS += $(INC_FLAGS) -Wall -O2 -D_FORTIFY_SOURCE=2 -Wformat-security \
	  -fPIE -Wextra -Wpedantic -Wcast-qual -Walloca \
	  -Wnull-dereference -Wstack-protector -Wvla -Wfloat-equal 	\
	  -Wbad-function-cast -Wpointer-arith -fstack-protector-strong \
	  -Wuninitialized -Wvla -Wpedantic -Wno-unused-command-line-argument

GCC_FLAGS = -mshstk #-fanalyzer 

CLANG_FLAGS = -fstack-clash-protection -Wthread-safety -Wthread-safety-beta \
	      -Widiomatic-parentheses -Wpointer-arith -Wunreachable-code-aggressive \
	      -Wconditional-uninitialized  -Warray-bounds-pointer-arithmetic 	\
	      -Wshift-sign-overflow -Wcomma  

LDFLAGS ?= $(addprefix -L,$(LIB_DIR)) -lpthread -lssl -lcrypto \
	   -pie -Wl,-z,relro -Wl,-z,now -Wl,-z,noexecstack -Wl,-z,separate-code
	   
ifeq ($(shell $(CC) -v 2>&1 | grep -c "gcc version"),1)
  CFLAGS += $(GCC_FLAGS)
else ifeq ($(shell $(CC) -v 2>&1 | grep -c "clang version"),1)
  CFLAGS += $(CLANG_FLAGS)  
  #LDFLAGS += -fsanitize=safe-stack
endif

$(BIN_DIR)/$(TARGET_EXEC): $(OBJS)
	$(CC) $(OBJS) -o $@ $(LDFLAGS)

$(BUILD_DIR)/%.c.o: %.c
	$(MKDIR_P) $(dir $@)
	$(CC) $(CFLAGS) -c $< -o $@

ifeq ($(shell $(CC) -v 2>&1 | grep -c "clang version"),1)
debug: LDFLAGS += -fsanitize=address -fsanitize=leak -fno-omit-frame-pointer -fsanitize=undefined -fsanitize=float-divide-by-zero -fsanitize=float-cast-overflow -fsanitize=integer 
endif 
debug: CFLAGS += -g -DDEBUG
debug: LDFLAGS += -g -DDEBUG 
debug: $(BIN_DIR)/$(TARGET_EXEC)

.PHONY: clean

clean:
	$(RM) $(BIN_DIR)/$(TARGET_EXEC)
	$(RM) -r $(BUILD_DIR)

-include $(DEPS)

MKDIR_P ?= mkdir -p
