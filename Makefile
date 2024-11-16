# Project name
NAME = mybot

# Directories
SRC_DIR = srcs
INC_DIR = include
OUT_DIR = build
BIN_DIR = bin
LIB_DIR = libs/json/include

# Source files
SRC_FILES = main

# Compiler
CXX = clang++

# Detect environment
ifeq ($(shell uname -s), Darwin)  # macOS (로컬 환경)
    DPP_INC_DIR = /opt/homebrew/include
    DPP_LIB_DIR = /opt/homebrew/lib
    EXTRA_INCLUDES = -I/opt/homebrew/opt/llvm/include
    EXTRA_LIBS = -lcurl
else  # Replit 환경
    DPP_INC_DIR = ./usr/include
    DPP_LIB_DIR = ./usr/lib
    EXTRA_INCLUDES =
    EXTRA_LIBS =
endif

# Include paths
INCLUDES = -I$(INC_DIR) \
           -I$(LIB_DIR) \
           -I$(DPP_INC_DIR) \
           $(EXTRA_INCLUDES)

# Library paths and flags
LIBS = -L$(DPP_LIB_DIR) \
       -ldpp \
       -lpthread \
       -lssl \
       -lcrypto \
       -lopus \
       -lz \
       $(EXTRA_LIBS)

# Compile flags
CXXFLAGS = -std=c++17 -g3 $(INCLUDES)

# Linker flags
LDFLAGS = $(LIBS)

# Source and Object files
SRCS = $(addprefix $(SRC_DIR)/, $(addsuffix .cpp, $(SRC_FILES)))
OBJS = $(SRCS:$(SRC_DIR)/%.cpp=$(OUT_DIR)/%.o)

# Rules
all: $(NAME)

clean:
	$(RM) -r $(OUT_DIR)

fclean: clean
	$(RM) -r $(BIN_DIR)
	$(RM) $(NAME)

re: fclean all

.PHONY: all clean fclean re

# Build rules
$(NAME): $(OBJS)
	@mkdir -p $(BIN_DIR)
	$(CXX) $(OBJS) $(LDFLAGS) -o $(BIN_DIR)/$(NAME)

$(OBJS): $(OUT_DIR)/%.o: $(SRC_DIR)/%.cpp
	@mkdir -p $(@D)
	$(CXX) $(CXXFLAGS) -c $< -o $@