dist/SYSCTRL0.BIN: systemctrl/systemctrl.prx
	$(Q)psp-fixup-imports -m ./common/nidmap.txt $<
	$(Q)python ./contrib/PC/pspgz/pspgz.py $(patsubst %.prx,%.gz.prx,$<) contrib/PC/pspgz/SystemControl.hdr $< SystemControl 0x3007
	$(Q)cp $(patsubst %.prx,%.gz.prx,$<) $@
	$(Q)rm -f $(patsubst %.prx,%.gz.prx,$<) $(patsubst %.prx,%.enc.prx,$<)

dist/GALAXY00.BIN: ISODrivers/galaxy/galaxy.prx
	$(Q)psp-fixup-imports -m ./common/nidmap.txt $<
	$(Q)python ./contrib/PC/pspgz/pspgz.py $(patsubst %.prx,%.gz.prx,$<) contrib/PC/pspgz/SystemControl.hdr $< PROGalaxyController 0x1006
	$(Q)cp $(patsubst %.prx,%.gz.prx,$<) $@
	$(Q)rm -f $(patsubst %.prx,%.gz.prx,$<) $(patsubst %.prx,%.enc.prx,$<)

dist/INFERNO0.BIN: ISODrivers/inferno/inferno.prx
	$(Q)psp-fixup-imports -m ./common/nidmap.txt $<
	$(Q)python ./contrib/PC/pspgz/pspgz.py $(patsubst %.prx,%.gz.prx,$<) contrib/PC/pspgz/SystemControl.hdr $< PRO_Inferno_Driver 0x1006
	$(Q)cp $(patsubst %.prx,%.gz.prx,$<) $@
	$(Q)rm -f $(patsubst %.prx,%.gz.prx,$<) $(patsubst %.prx,%.enc.prx,$<)

dist/STARGATE.BIN: stargate/stargate.prx
	$(Q)psp-fixup-imports -m ./common/nidmap.txt $<
	$(Q)python ./contrib/PC/pspgz/pspgz.py $(patsubst %.prx,%.gz.prx,$<) contrib/PC/pspgz/SystemControl.hdr $< stargate 0x1007
	$(Q)cp $(patsubst %.prx,%.gz.prx,$<) $@
	$(Q)rm -f $(patsubst %.prx,%.gz.prx,$<) $(patsubst %.prx,%.enc.prx,$<)

dist/EXITGAME.BIN: exitgame/exitgame.prx
	$(Q)psp-fixup-imports -m ./common/nidmap.txt $<
	$(Q)python ./contrib/PC/pspgz/pspgz.py $(patsubst %.prx,%.gz.prx,$<) contrib/PC/pspgz/SystemControl.hdr $< exitgame 0x1007
	$(Q)cp $(patsubst %.prx,%.gz.prx,$<) $@
	$(Q)rm -f $(patsubst %.prx,%.gz.prx,$<) $(patsubst %.prx,%.enc.prx,$<)

dist/POPCORN0.BIN: popcorn/popcorn.prx
	$(Q)psp-fixup-imports -m ./common/nidmap.txt $<
	$(Q)python ./contrib/PC/pspgz/pspgz.py $(patsubst %.prx,%.gz.prx,$<) contrib/PC/pspgz/SystemControl.hdr $< PROPopcornManager 0x1006
	$(Q)cp $(patsubst %.prx,%.gz.prx,$<) $@
	$(Q)rm -f $(patsubst %.prx,%.gz.prx,$<) $(patsubst %.prx,%.enc.prx,$<)

dist/MARCH330.BIN: ISODrivers/march33/march33.prx
	$(Q)python ./contrib/PC/pspgz/pspgz.py $(patsubst %.prx,%.gz.prx,$<) contrib/PC/pspgz/SystemControl.hdr $< pspMarch33_Driver 0x1006
	$(Q)cp $(patsubst %.prx,%.gz.prx,$<) $@
	$(Q)rm -f $(patsubst %.prx,%.gz.prx,$<) $(patsubst %.prx,%.enc.prx,$<)

dist/MEDIASYN.BIN: contrib/PSP/f0-kd-mediasync.prx
	$(Q)python ./contrib/PC/pspgz/pspgz.py $(patsubst %.prx,%.gz.prx,$<) contrib/PC/pspgz/SystemControl.hdr $< pspMarch33_Driver 0x1006
	$(Q)cp $(patsubst %.prx,%.gz.prx,$<) $@
	$(Q)rm -f $(patsubst %.prx,%.gz.prx,$<) $(patsubst %.prx,%.enc.prx,$<)

dist/NPDRM.PRX:  contrib/PSP/psp-npdrm.prx
	$(Q)python ./contrib/PC/pspgz/pspgz.py $(patsubst %.prx,%.gz.prx,$<) contrib/PC/pspgz/SystemControl.hdr $< scePspNpDrm_Driver 0x1006
	$(Q)cp $(patsubst %.prx,%.gz.prx,$<) $@
	$(Q)rm -f $(patsubst %.prx,%.gz.prx,$<) $(patsubst %.prx,%.enc.prx,$<)

dist/MODULEMR.BIN: contrib/PSP/f0-kd-modulemgr.prx
	$(Q)python ./contrib/PC/pspgz/pspgz.py $(patsubst %.prx,%.gz.prx,$<) contrib/PC/pspgz/SystemControl.hdr $< sceModuleManager 0x1006
	$(Q)cp $(patsubst %.prx,%.gz.prx,$<) $@
	$(Q)rm -f $(patsubst %.prx,%.gz.prx,$<) $(patsubst %.prx,%.enc.prx,$<)

dist/NP966000.BIN: contrib/PSP/f0-kd-np9660.prx
	$(Q)python ./contrib/PC/pspgz/pspgz.py $(patsubst %.prx,%.gz.prx,$<) contrib/PC/pspgz/SystemControl.hdr $< sceNp9660_driver 0x1006
	$(Q)cp $(patsubst %.prx,%.gz.prx,$<) $@
	$(Q)rm -f $(patsubst %.prx,%.gz.prx,$<) $(patsubst %.prx,%.enc.prx,$<)

dist/INTERRUP.BIN: contrib/PSP/f0-kd-interruptman.prx
	$(Q)python ./contrib/PC/pspgz/pspgz.py $(patsubst %.prx,%.gz.prx,$<) contrib/PC/pspgz/SystemControl.hdr $< sceInterruptManager 0x1006
	$(Q)cp $(patsubst %.prx,%.gz.prx,$<) $@
	$(Q)rm -f $(patsubst %.prx,%.gz.prx,$<) $(patsubst %.prx,%.enc.prx,$<)

ifeq ($(POPSMAN_VERSION), VERSION_PSP)
POPSMAN_BINARY = contrib/PSP/popsman_psp.prx
else
POPSMAN_BINARY = contrib/PSP/f0-kd-popsman.prx
endif

dist/POPSMAN0.BIN: $(POPSMAN_BINARY)
	$(Q)python ./contrib/PC/pspgz/pspgz.py $(patsubst %.prx,%.gz.prx,$<) contrib/PC/pspgz/SystemControl.hdr $< scePops_Manager 0x1007
	$(Q)cp $(patsubst %.prx,%.gz.prx,$<) $@
	$(Q)rm -f $(patsubst %.prx,%.gz.prx,$<) $(patsubst %.prx,%.enc.prx,$<)

ifeq ($(POPS_VERSION), VERSION_PSP)
POPS_BINARY = contrib/PSP/pops_psp.prx
else
POPS_BINARY = contrib/PSP/pops.prx
endif

dist/POPS.PRX: $(POPS_BINARY)
	$(Q)python ./contrib/PC/pspgz/pspgz.py $(patsubst %.prx,%.gz.prx,$<) contrib/PC/pspgz/UserModule.hdr $< pops 0x0000
	$(Q)cp $(patsubst %.prx,%.gz.prx,$<) $@
	$(Q)rm -f $(patsubst %.prx,%.gz.prx,$<) $(patsubst %.prx,%.enc.prx,$<)

dist/PSPVMC00.BIN: contrib/PSP/f0-vsh-module-libpspvmc.prx
	$(Q)python ./contrib/PC/pspgz/pspgz.py $(patsubst %.prx,%.gz.prx,$<) contrib/PC/pspgz/UserModule.hdr $< pspvmc_Library 0x0000
	$(Q)cp $(patsubst %.prx,%.gz.prx,$<) $@
	$(Q)rm -f $(patsubst %.prx,%.gz.prx,$<) $(patsubst %.prx,%.enc.prx,$<)

dist/IMPOSE.BIN: contrib/PSP/impose_05g.prx
	$(Q)python ./contrib/PC/pspgz/pspgz.py $(patsubst %.prx,%.gz.prx,$<) contrib/PC/pspgz/SystemControl.hdr $< sceImpose_Driver 0x1007
	$(Q)cp $(patsubst %.prx,%.gz.prx,$<) $@
	$(Q)rm -f $(patsubst %.prx,%.gz.prx,$<) $(patsubst %.prx,%.enc.prx,$<)
