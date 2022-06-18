PSPSDK=$(shell psp-config --pspsdk-path)
INCDIR = $(PSPSDK)/include $(PROVITA)/common/include $(PROVITA)
LIBS= -ldebug -lansic -L $(PROVITA)/libs
TARGET = ark.bin

all: pre-build $(TARGET)

CFLAGS = -Wall -Os -G0 -fno-pic $(addprefix -I, $(INCDIR))

CFLAGS += -I $(PROVITA)/exploit/kxploit/ -I $(PROVITA)/systemctrl/include
CFLAGS += -I $(PROVITA)/systemctrl/include -DKXPLOIT_LOADADDR=$(KXPLOIT_LOADADDR)

C_OBJS = \
	main.o \
	reboot.o \
	210patch.o \
	flashpatch.o \
	$(PROVITA)/common/functions.o \
	$(PROVITA)/systemctrl/src/loadexec_patch.o

include $(PROVITA)/common/make/global.mak
include $(PROVITA)/common/make/beauty_bin.mak
include $(PROVITA)/common/make/exploit_config.mak

LINKFILE = link.l

ifdef DEBUG
CFLAGS += -DDEBUG=$(DEBUG)
endif

CC = psp-gcc
LD = psp-ld
STRIP = psp-strip
OBJCOPY = psp-objcopy

pre-build: link.l

cmd_genlink = python $(PROVITA)/contrib/PC/genlink/genlink.py $(EXPLOIT_LOADADDR) exploitEntry > $(LINKFILE)

link.l:
	$(Q)$(cmd_genlink)

main.elf: $(C_OBJS)

$(TARGET): main.elf
	$(Q)$(STRIP) -s $<
	$(Q)$(OBJCOPY) -O binary $< $@
	@echo GET $@

clean:
	$(Q)rm -rf *~ *.s *.o *.elf ark.bin exploit.h link.l
