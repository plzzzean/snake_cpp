# Makefile
# snake_cpp2 프로젝트의 실행 파일과 로직 테스트 빌드 규칙을 정의한다.

CXX := g++

CXXFLAGS := -std=c++17 -Wall -Wextra -pedantic -Isrc
LDFLAGS := -lncurses

TARGET := snake
SRC := $(wildcard src/*.cpp)
BUILD_DIR := build
OBJ := $(patsubst src/%.cpp,$(BUILD_DIR)/%.o,$(SRC))
TEST_TARGET := $(BUILD_DIR)/snake_tests
TEST_SRC := tests/test_logic.cpp src/DynamicWall.cpp src/Food.cpp src/Gate.cpp src/Map.cpp src/Poison.cpp src/Shield.cpp src/Snake.cpp

.PHONY: all run test clean

all: $(TARGET)

$(TARGET): $(OBJ)
	$(CXX) $(OBJ) -o $@ $(LDFLAGS)

$(BUILD_DIR)/%.o: src/%.cpp | $(BUILD_DIR)
	$(CXX) $(CXXFLAGS) -c $< -o $@

$(BUILD_DIR):
	mkdir -p $(BUILD_DIR)

run: $(TARGET)
	./$(TARGET)

test: $(TEST_TARGET)
	./$(TEST_TARGET)

$(TEST_TARGET): $(TEST_SRC) | $(BUILD_DIR)
	$(CXX) $(CXXFLAGS) $(TEST_SRC) -o $@

clean:
	rm -rf $(BUILD_DIR) $(TARGET) snake_tests
