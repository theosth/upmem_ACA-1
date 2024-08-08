# Configuration variables
DPU_COMPILER := dpu-upmem-dpurte-clang
HOST_COMPILER := gcc
HOST_COMPILER_FLAGS := --std=c99
DPU_PKG_CONFIG := dpu-pkg-config
NR_TASKLETS := 4

# Directories
TEST_DIR := Tests/Helloworld
DEST_DIR := dest
LOG_DIR := $(DEST_DIR)/logs
BUILD_DIR := $(DEST_DIR)/build

# Source files
DPU_SOURCE := $(TEST_DIR)/helloworld_own_example3.c
HOST_SOURCE := $(TEST_DIR)/helloworld_host_own.c

# Output files
DPU_TARGET := $(BUILD_DIR)/helloworld_own_example3
HOST_TARGET := $(BUILD_DIR)/helloworld_host_own

# Timestamp for logs
TIMESTAMP := $(shell date +'%Y-%m-%d_%H-%M-%S')

# Default target
all: setup compile

# Setup
setup:
	./setup.sh
	./source_wrapper.sh
	mkdir -p $(LOG_DIR) $(BUILD_DIR)

# Compilation
compile: $(DPU_TARGET) $(HOST_TARGET)

# Compile DPU program
$(DPU_TARGET): $(DPU_SOURCE)
	$(DPU_COMPILER) -DNR_TASKLETS=$(NR_TASKLETS) -o $@ $< 2>&1 | tee $(LOG_DIR)/dpu_compile_$(TIMESTAMP).log

# Compile host program
$(HOST_TARGET): $(HOST_SOURCE)
	cd $(TEST_DIR) && $(HOST_COMPILER) $(HOST_COMPILER_FLAGS) $(notdir $<) -o $(abspath $@) `$(DPU_PKG_CONFIG) --cflags --libs dpu` 2>&1 | tee $(abspath $(LOG_DIR)/host_compile_$(TIMESTAMP).log)

# Run host program
run: compile
	cd $(BUILD_DIR) && UPMEM_VERBOSE=1 ./$(notdir $(HOST_TARGET)) ./$(notdir $(DPU_TARGET)) 2>&1 | tee $(abspath $(LOG_DIR)/run_$(TIMESTAMP).log)

# Debug with dpu-lldb
debug: compile
	@echo "Starting dpu-lldb session..."
	@echo "file $(DPU_TARGET)" > .lldb_commands
	@echo "process launch" >> .lldb_commands
	@dpu-lldb -s .lldb_commands 2>&1 | tee $(LOG_DIR)/debug_$(TIMESTAMP).log
	@rm .lldb_commands

# Clean up build files
clean:
	rm -rf $(BUILD_DIR)/*

# Clean up everything including logs
clean-all:
	rm -rf $(DEST_DIR)/*

.PHONY: all setup compile run debug clean clean-all
