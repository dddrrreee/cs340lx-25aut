# need the makefile to give an error if this doesn't exist
PROGS := 0-fill-screen.c
# PROGS := 1-draw-commands.c

# COMMON_SRC := sh1106-display-driver.c
COMMON_SRC += standard-ascii-font.c
# COMMON_SRC += i2c.c

PREBUILT_OBJS := i2c.o
PREBUILT_OBJS += sh1106-display-driver-staff.o

# uncomment if you want it to automatically run.
RUN=1

# define this if you need to give the device for your pi
TTYUSB = 
BOOTLOADER = pi-install
RUN = 1

include $(CS340LX_2025_PATH)/libpi/mk/Makefile.robust
