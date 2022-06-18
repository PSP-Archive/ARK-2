TARGET_LIB=libcolordebugger.a

C_OBJS = \
colordebugger.o

all: $(TARGET_LIB)

OBJS = $(C_OBJS)
PSPSDK=$(shell psp-config --pspsdk-path)

CC=psp-gcc
INCDIR = $(PROVITA)/common/include
CFLAGS = -Wall -Os -G0 -fno-pic
CXXFLAGS = $(CFLAGS) -fno-exceptions -fno-rtti
ASFLAGS = $(CFLAGS)
LIBDIR =

include $(PROVITA)/common/make/global.mak
include $(PROVITA)/common/make/beauty_lib.mak
