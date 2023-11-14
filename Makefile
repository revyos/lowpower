##
 # Copyright (C) 2023 Alibaba Group Holding Limited
##

test = $(shell if [ -f "../.param" ]; then echo "exist"; else echo "noexist"; fi)
ifeq ("$(test)", "exist")
  include ../.param
endif

SDK_VER=v0.1

CONFIG_DEBUG_MODE=1
CONFIG_OUT_ENV=riscv_linux

CONFIG_BUILD_DRV_EXTRA_PARAM:=""

export PATH_TO_SYSROOT=${SYSROOT_DIR}
export TOOLSCHAIN_PATH=${TOOLCHAIN_DIR}
export TOOLCHAIN_HOST=${CROSS_COMPILE}
export PATH_TO_BUILDROOT=$(BUILDROOT_DIR)

DIR_TARGET_FIRMWARE=firmware

BUILD_LOG_START="\033[47;30m>>> $(TARGET_LOWPOWER) $@ begin\033[0m"
BUILD_LOG_END  ="\033[47;30m<<< $(TARGET_LOWPOWER) $@ end\033[0m"

#
# Do a parallel build with multiple jobs, based on the number of CPUs online
# in this system: 'make -j8' on a 8-CPU system, etc.
#
# (To override it, run 'make JOBS=1' and similar.)
#
ifeq ($(JOBS),)
  JOBS := $(shell grep -c ^processor /proc/cpuinfo 2>/dev/null)
  ifeq ($(JOBS),)
    JOBS := 1
  endif
endif

all:    info firmware install_local_output
.PHONY: info firmware install_local_output \
        install_prepare clean_firmware clean_output clean

info:
	@echo $(BUILD_LOG_START)
	@echo "  ====== Build Info from repo project ======"
	@echo "    BUILD_SYSTEM="$(BUILD_SYSTEM)
	@echo "    BUILDROOT_DIR="$(BUILDROOT_DIR)
	@echo "    SYSROOT_DIR="$(SYSROOT_DIR)
	@echo "    TOOLCHAIN_DIR="$(TOOLCHAIN_DIR)
	@echo "    CROSS_COMPILE="$(CROSS_COMPILE)
	@echo "    LINUX_DIR="$(LINUX_DIR)
	@echo "    ARCH="$(ARCH)
	@echo "    KBUILD_CFLAGS="$(KBUILD_CFLAGS)
	@echo "    KBUILD_AFLAGS="$(KBUILD_AFLAGS)
	@echo "    KBUILD_LDFLAGS="$(KBUILD_LDFLAGS)
	@echo "    BOARD_NAME="$(BOARD_NAME)
	@echo "    KERNEL_ID="$(KERNELVERSION)
	@echo "    KERNEL_DIR="$(LINUX_DIR)
	@echo "    CC="$(CC)
	@echo "    CXX="$(CXX)
	@echo "    CPP="$(CPP)
	@echo "    OBJCOPY="$(OBJCOPY)
	@echo "    AS="$(AS)
	@echo "    LD="$(LD)
	@echo "  ====== Build configuration by settings ======"
	@echo "    TARGET_LOWPOWER="$(TARGET_LOWPOWER)
	@echo "    CONFIG_DEBUG_MODE="$(CONFIG_DEBUG_MODE)
	@echo "    CONFIG_OUT_ENV="$(CONFIG_OUT_ENV)
	@echo "    JOBS="$(JOBS)
	@echo "    SDK_VERSION="$(SDK_VER)
	@echo $(BUILD_LOG_END)

firmware:
	@echo $(BUILD_LOG_START)
	make -C firmware CROSS=$(CROSS_COMPILE) ARCH=$(ARCH)
	@echo $(BUILD_LOG_END)

clean_firmware:
	@echo $(BUILD_LOG_START)
	make -C firmware clean
	@echo $(BUILD_LOG_END)

install_prepare:
	mkdir -p ./output/$(DIR_TARGET_FIRMWARE)

install_local_output: firmware install_prepare
	@echo $(BUILD_LOG_START)
	find ./firmware -name "*.elf" | xargs -i cp -f {} ./output/$(DIR_TARGET_FIRMWARE)
	find ./firmware -name "*.bin" | xargs -i cp -f {} ./output/$(DIR_TARGET_FIRMWARE)
	find ./firmware -name "*.map" | xargs -i cp -f {} ./output/$(DIR_TARGET_FIRMWARE)
	@if [ `command -v tree` != "" ]; then \
	    tree ./output;             \
	fi
	@echo $(BUILD_LOG_END)

clean_output:
	@echo $(BUILD_LOG_START)
	rm -rf ./output
	@echo $(BUILD_LOG_END)

clean: clean_output clean_firmware

