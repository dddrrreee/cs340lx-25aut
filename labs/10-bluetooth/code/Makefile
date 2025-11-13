PROGS = 1-test-pl011.c
# PROGS = 2-firmware-upload.c
# PROGS = 3-read-bdaddr.c
# PROGS = 5-connect.c
# PROGS = 5-accept-connection.c

COMMON_SRC += bt.c
# COMMON_SRC += BCM43430A1.c

# Replace with your own pl011.c!
# COMMON_SRC += pl011.c
STAFF_OBJS += staff-pl011.o

# if you need to override the install
BOOTLOADER = my-install

OPT_LEVEL = -Og

# automatically run the code
RUN = 1

include $(CS340LX_2025_PATH)/libpi/mk/Makefile.robust
