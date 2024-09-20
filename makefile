# 定義編譯器
CXX = g++

# 定義編譯選項
CXXFLAGS = -lSDL2

# 定義目標檔案
TARGET = goldminer

# 定義原始碼檔案
SRC = goldminer.cpp

# 預設目標，編譯並執行
all: $(TARGET)
	./$(TARGET)

# 編譯過程
$(TARGET): $(SRC)
	$(CXX) $(SRC) -o $(TARGET) $(CXXFLAGS)

# 清理編譯過程產生的檔案
clean:
	rm -f $(TARGET)
