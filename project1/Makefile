.PHONY: build run clean help

BUILD_DIR := build
EXECUTABLE := $(BUILD_DIR)/pipeline
VIDEO ?= sample.mp4

help:
	@echo "=== PROJECT 1 Makefile ==="
	@echo ""
	@echo "Usage:"
	@echo "  make build              - Build the project"
	@echo "  make run                - Build and run (default video: sample.mp4)"
	@echo "  make run VIDEO=video.mp4 - Build and run with custom video"
	@echo "  make clean              - Clean build artifacts"
	@echo "  make help               - Show this help"
	@echo ""
	@echo "Example:"
	@echo "  make run VIDEO=~/Videos/test.mp4"

build:
	@echo "🔨 Building..."
	@mkdir -p $(BUILD_DIR)
	@cd $(BUILD_DIR) && cmake -DCMAKE_BUILD_TYPE=Release .. && make
	@echo "✅ Build complete: $(EXECUTABLE)"

run: build
	@$(EXECUTABLE)

clean:
	@echo "🧹 Cleaning..."
	@rm -rf $(BUILD_DIR)
	@echo "✅ Clean complete"

.DEFAULT_GOAL := help
