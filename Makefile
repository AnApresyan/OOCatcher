# Makefile for building C++ raylib project with CMake

.PHONY: all build run clean

BUILD_DIR = build

all: build

build:
	mkdir -p $(BUILD_DIR)
	cd $(BUILD_DIR) && cmake .. && cmake --build .

run: build
	cd $(BUILD_DIR) && ./OOCatcher

clean:
	rm -rf $(BUILD_DIR)
