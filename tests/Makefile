# SPDX-License-Identifier: Apache-2.0
CMSIS_INCLUDE ?=
BUILD_DIR ?= /tmp/hal-py32-tests
DEVICES := PY32F002Ax5 PY32F003x4 PY32F030x6

.PHONY: run
run:
	@test -f "$(CMSIS_INCLUDE)/core_cm0plus.h" || { \
		echo "Pass CMSIS_INCLUDE=/path/to/CMSIS/Core/Include." >&2; \
		exit 1; \
	}
	@mkdir -p $(BUILD_DIR)
	@set -e; for device in $(DEVICES); do \
		$(CC) -std=c11 -Werror=implicit-function-declaration \
			-Wno-int-to-pointer-cast -Wno-pointer-to-int-cast \
			-D$$device -DFORBID_VECT_TAB_MIGRATION -I$(CMSIS_INCLUDE) \
			-I../py32/py32f0xx/drivers/include \
			test_registers.c -o $(BUILD_DIR)/$$device; \
		$(BUILD_DIR)/$$device; \
	done
