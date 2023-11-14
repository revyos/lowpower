#
# SPDX-License-Identifier: BSD-2-Clause
#
# Copyright (c) 2023 Alibaba Corporation.
#
# Authors: David Li <liyong.li@alibaba-inc.com>
#

SRCDIR = .
INCDIR = $(SRCDIR)/include
OUTDIR = ../output


# Setup compilation commands flags
GENFLAGS    +=  -I$(INCDIR)
GENFLAGS    +=  $(firmware-genflags-y)

CFLAGS      =   -g -Wall -Werror -ffreestanding -nostdlib -fno-strict-aliasing -O2
CFLAGS      +=  -fno-omit-frame-pointer -fno-optimize-sibling-calls
CFLAGS      +=  -mno-save-restore -mstrict-align
CFLAGS      +=  -mabi=lp64 -march=rv64im
CFLAGS      +=  -mcmodel=medany
CFLAGS		+=  -D__KERNEL__
CFLAGS      +=  $(GENFLAGS)
#CFLAGS      +=  -fno-pie -no-pie
CFLAGS      +=  $(firmware-cflags-y)

CPPFLAGS    =	$(GENFLAGS)
CPPFLAGS	+=  -E -D__KERNEL__
CPPFLAGS    +=  $(firmware-cppflags-y)

ASFLAGS     =   -g -Wall
ASFLAGS     +=  -mabi=lp64 -march=rv64im
ASFLAGS     +=  $(GENFLAGS)
ASFLAGS     +=  $(firmware-asflags-y)

LDFLAGS		=	-m elf64lriscv -Bstatic --gc-sections --no-dynamic-linker
LDFLAGS		+=	

BIN = str.bin
ELF = str.elf
MAP = str.map
INCS=-I ./include -I ./include/asm
SRCS:=$(wildcard *.c)
COBJS:=$(SRCS:.c=.o)
ASRCS:=$(wildcard *.S)
AOBJS:=$(ASRCS:.S=.S.o)
LDSSRCS:=$(wildcard *.ldS)
LDSOBJS:=$(LDSSRCS:.ldS=.ld)

all: $(BIN)

$(BIN): $(ELF)
	@echo "make BIN"
	$(OBJCOPY) -S -O binary $(ELF) $(BIN)

$(ELF): $(COBJS) $(AOBJS) $(LDSOBJS)
	@echo "make ELF"
	$(LD) $(LDFLAGS) $(COBJS) $(AOBJS) -T $(LDSOBJS) -o $(ELF) -Map $(MAP)

$(LDSOBJS): %.elf.ld: %.elf.ldS
	@echo "CPP="$(CPP)
	$(CPP) $(CPPFLAGS) -x c $< | grep -v "\#" > $@

$(COBJS): %.o: %.c
	@echo "CC="$(CC)
	$(CC) $(CFLAGS) -c $< -o $@ $(INCS)

$(AOBJS): %.S.o: %.S
	@echo "AS="$(AS)
	@echo "ASFLAGS="$(ASFLAGS)
	$(AS) $(ASFLAGS) -c $< -o $@ $(INCS)

clean:
	@echo "driver clean"
	rm -rf $(BIN) $(ELF) $(MAP) $(COBJS) $(AOBJS) $(LDSOBJS)
