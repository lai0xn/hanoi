CXX := g++
CXXFLAGS := -std=c++17 -Wall \
            -I./imgui -I./imgui/backends -I./include \
            -I/opt/homebrew/include

LDFLAGS := -L/opt/homebrew/lib -lglfw \
           -framework Cocoa -framework IOKit -framework CoreVideo -framework OpenGL

SRC_DIR := src
IMGUI_DIR := imgui
BACKENDS_DIR := imgui/backends
OBJ_DIR := obj
BIN_DIR := bin

# Project source files
SRC := $(wildcard $(SRC_DIR)/*.cpp)

# ImGui core + backend
IMGUI_SRC := \
    $(IMGUI_DIR)/imgui.cpp \
    $(IMGUI_DIR)/imgui_draw.cpp \
    $(IMGUI_DIR)/imgui_widgets.cpp \
    $(IMGUI_DIR)/imgui_tables.cpp \
    $(IMGUI_DIR)/imgui_demo.cpp \
    $(BACKENDS_DIR)/imgui_impl_glfw.cpp \
    $(BACKENDS_DIR)/imgui_impl_opengl3.cpp

OBJS := $(SRC:$(SRC_DIR)/%.cpp=$(OBJ_DIR)/%.o) \
        $(IMGUI_SRC:%.cpp=$(OBJ_DIR)/%.o)

TARGET := $(BIN_DIR)/app

all: $(TARGET)

$(TARGET): $(OBJS)
	@mkdir -p $(BIN_DIR)
	$(CXX) $^ -o $@ $(LDFLAGS)

$(OBJ_DIR)/%.o: $(SRC_DIR)/%.cpp
	@mkdir -p $(OBJ_DIR)
	$(CXX) $(CXXFLAGS) -c $< -o $@

$(OBJ_DIR)/%.o: %.cpp
	@mkdir -p $(OBJ_DIR)
	$(CXX) $(CXXFLAGS) -c $< -o $@

clean:
	rm -rf $(OBJ_DIR) $(BIN_DIR)

run: all
	./$(TARGET)

.PHONY: all clean run

