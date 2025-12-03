PROGS := 1-pin-setup.c
# PROGS := 2-camera-setup.c
# PROGS := 3-camera-display.c

# COMMON_SRC += ov7670_init.c
# COMMON_SRC += ov7660.c
PREBUILT_OBJS := ov7670_staff.o
PREBUILT_OBJS += mbox_staff.o
PREBUILT_OBJS += i2c_staff.o

# define this if you need to give the device for your pi
TTYUSB = 
BOOTLOADER = my-install

# uncomment if you want it to automatically run.
RUN=1

include $(CS340LX_2025_PATH)/libpi/mk/Makefile.robust
