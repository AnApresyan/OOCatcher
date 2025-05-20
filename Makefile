.PHONY: all build run clean say

BUILD_DIR = build

all: build

build:
	mkdir -p $(BUILD_DIR)
	cd $(BUILD_DIR) && cmake .. && cmake --build .

run: build
	cd $(BUILD_DIR) && ./OOCatcher

clean:
	rm -rf $(BUILD_DIR)

say:
	$(MAKE) clean
	$(MAKE) build
	$(MAKE) run
