PSPSDK=$(shell psp-config --pspsdk-path)
PSPDIR=$(shell psp-config --psp-prefix)
TARGET_LIB = libdebug.a
OBJS = lib.o graphics.o font.o

all: $(TARGET_LIB)
CFLAGS = -O2 -G0 -Wall -s -lstdc++ -I $(PROVITA)/common/include
CXXFLAGS = $(CFLAGS) -fexceptions -fno-rtti
PSP_FW_VERSION=660

include $(PROVITA)/common/make/global.mak
include $(PSPSDK)/lib/build.mak
include $(PROVITA)/common/make/beauty_lib.mak
