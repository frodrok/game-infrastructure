
BUILD_TYPE := debug
OUTPUT_DIR := target
BINARY_NAME := server

CC := g++
GCC_FLAGS := -std=c++2a -Wall
GCC_INPUT := src/cpp/main.cpp

INCLUDE_DIR := include

LIBS_DIR := lib

LINK_LIBS := -lcurl

init:
	mkdir -p ./$(OUTPUT_DIR)/$(BUILD_TYPE)

build:
	g++ -L$(LIBS_DIR) -I$(INCLUDE_DIR) $(GCC_FLAGS) $(GCC_INPUT) -o ./$(OUTPUT_DIR)/$(BUILD_TYPE)/$(BINARY_NAME) $(LINK_LIBS)

run:
	./$(OUTPUT_DIR)/$(BUILD_TYPE)/$(BINARY_NAME)

clean:
	rf -rm ./$(OUTPUT_DIR)

all: init build run

