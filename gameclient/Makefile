
CC := g++

BUILD_TYPE := debug

SOURCE_FILES := src/cpp/unix/main.cpp

BINARY_NAME := client

INCLUDE_DIR := include

init:
	mkdir -p ./target/$(BUILD_TYPE)

build:
	$(CC) -I$(INCLUDE_DIR) $(SOURCE_FILES) -o target/$(BUILD_TYPE)/$(BINARY_NAME) 

run:
	./target/$(BUILD_TYPE)/$(BINARY_NAME)

all: init build run
