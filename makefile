CC = g++
BUILD_DIR = build
INCLUDE_DIR = include
LIB_DIR = lib
SRC_DIR = src

# 定義頭文件指令
CXXFLAGS = -I$(INCLUDE_DIR)
# 定義庫文件指令
LDFLAGS = -Llib/SDL2
# 鏈接庫文件指令
LDLIBS = -lmingw32 -lSDL2main -lSDL2

# 獲取所有的源文件列表
CPP_FILES = $(wildcard $(SRC_DIR)/*.cpp)
# 從源文件列表獲取目標文件列表
OBJ_FILES = $(patsubst $(SRC_DIR)/%.cpp,$(BUILD_DIR)/%.o,$(CPP_FILES))

# 默認目標：生成main文件
all: main

main: $(OBJ_FILES)
	$(CC) $(LDFLAGS) -o $@ $^ $(LDLIBS)

# 將每個源文件編譯成中間文件
$(BUILD_DIR)/%.o: $(SRC_DIR)/%.cpp
	$(CC) $(CXXFLAGS) -c $< -o $@

clean:
	del $(BUILD_DIR)\*.o main.exe

