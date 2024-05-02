STLINK_SERIAL = $(shell cat ../config.yaml | yq -r .$(TARGET).stlink.serial)
UART_DEVICE = $(shell cat ../config.yaml | yq -r .$(TARGET).uart.device)
BUILD_ID = $(TARGET) $(shell date +"%Y-%m-%d %H:%M:%S") git:$(shell git rev-parse HEAD)

ifeq ($(STLINK_SERIAL), "")
STFLASH_ARGS = '--serial $(STLINK_SERIAL)'
else
STFLASH_ARGS =
endif

C_DEFS += \
-DBUILD_ID="$(BUILD_ID)"


.PHONY: flash
flash: $(BUILD_DIR)/$(TARGET).bin _stlink-serial
	st-flash $(STFLASH_ARGS) write $(BUILD_DIR)/$(TARGET).bin 0x08000000

.PHONY: erase
erase: _stlink-serial
	st-flash $(STFLASH_ARGS) erase

.PHONY: reset
reset: _stlink-serial
	st-flash $(STFLASH_ARGS) reset

.PHONY: _stlink-serial
_stlink-serial:
	@if [ ! -n "$(STLINK_SERIAL)" ]; then \
		@echo "Stlink serial is not set. Check config.yaml file and put there one of the following serial numbers:"; \
		st-info --probe; \
		false; \
	fi

.PHONY:
serial: _uart-device
	picocom -b 115200 --echo "$(UART_DEVICE)"

.PHONY: _uart-device
_uart-device:
	@if [ ! -n "$(UART_DEVICE)" ]; then \
		@echo "UART device is not set. Check config.yaml file and put there one of devices from /dev/tty* or /dev/serial/by-path/*"; \
		ls /dev/serial/by-path/* /dev/tty*; \
		false; \
	fi

.PHONY: show-config
show-config:
	@echo "TARGET:           $(TARGET)"
	@echo "BUILD_ID (fresh): $(BUILD_ID)"
	@echo "UART_DEVICE:      $(UART_DEVICE)"
	@echo "STLINK_SERIAL:    $(STLINK_SERIAL)"
	@echo
