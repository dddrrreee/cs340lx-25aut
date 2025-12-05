CFLAGS_EXTRA  = -Iexternal-code

# PROGS := gpio_int_test.c
# COMMON_SRC += interrupt-asm.S

PROGS := doom.c

COMMON_SRC += external-code/mbox.c external-code/fat32.c external-code/mbr.c
COMMON_SRC += external-code/pi-sd.c external-code/mbr-helpers.c external-code/fat32-helpers.c 
COMMON_SRC += external-code/fat32-lfn-helpers.c external-code/unicode-utf8.c external-code/emmc.c


# define this if you need to give the device for your pi
TTYUSB = 
BOOTLOADER = my-install

# uncomment if you want it to automatically run.
RUN=1

# in case you need division.
LIBS += $(LIBM)
CFLAGS += $(LIBM_INC)

# Software floating point
# LIBS += $(CS340LX_2025_PATH)/lib/libgcc.a
# LIB_POST += $(CS340LX_2025_PATH)/lib/libgcc.a

# Hardware floating point
LIBS += $(CS340LX_2025_PATH)/lib/hardfp-libgcc.a
LIB_POST += $(CS340LX_2025_PATH)/lib/hardfp-libgcc.a

EXCLUDE ?= grep -v simple_boot
GREP_STR := 'HASH:\|ERROR:\|PANIC:\|PASS:\|TEST:'
include $(CS340LX_2025_PATH)/libpi/mk/Makefile.robust-v2

CFLAGS += -Wno-error