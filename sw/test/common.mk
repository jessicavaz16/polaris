ifeq ($(POLARIS_HOME), )
    $(error "POLARIS_HOME environment variable not set, did you source the sourceme script?")
endif

# Override these variables 
BUILD_DIR?= build
SRCS?= 
EXEC?= a.elf

################################################################################
RVPREFIX := riscv64-unknown-elf
CFLAGS += -Wall -O0
CFLAGS += -march=rv32i -mabi=ilp32 -nostartfiles -ffreestanding
LFLAGS := -T $(POLARIS_HOME)/sw/lib/link.ld 

all: build

.PHONY: build
build: $(BUILD_DIR)/$(EXEC)

$(BUILD_DIR)/$(EXEC): $(SRCS)
	mkdir -p $(BUILD_DIR)
	$(RVPREFIX)-gcc $(CFLAGS) $^ -o $@ $(LFLAGS)
	$(RVPREFIX)-objdump -dt $@ > $(basename $@).lst
	$(RVPREFIX)-objcopy -O binary $@ $(basename $@).bin
	xxd -e -c 4 $(basename $@).bin | awk '{print $$2}' > $(basename $@).hex

.PHONY: run
run: $(BUILD_DIR)/$(EXEC)
	@echo "Running $(EXEC)"
	polaris  $(basename $<).hex

.PHONY: clean
clean:
	rm -f $(BUILD_DIR)/*