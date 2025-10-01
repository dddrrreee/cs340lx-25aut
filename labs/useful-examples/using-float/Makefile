PROGS = complex-example.c fp-example.c
BOOTLOADER = my-install

# uncomment if you want it to automatically run.
RUN=1

# include libm and add to the compiler include flags.
LIBS += $(LIBM)
CFLAGS += $(LIBM_INC)

# specify libgcc.a twice as a hack to get around some 
# linking issue.
LIBS += $(CS340LX_2025_PATH)/lib/libgcc.a
LIB_POST += $(CS340LX_2025_PATH)/lib/libgcc.a

include $(CS340LX_2025_PATH)/libpi/mk/Makefile.robust-v2
