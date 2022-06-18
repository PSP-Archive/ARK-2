/*
 * This file is part of PRO CFW.

 * PRO CFW is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.

 * PRO CFW is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with PRO CFW. If not, see <http://www.gnu.org/licenses/ .
 */

#include <stdio.h>
#include <pspsdk.h>
#include <pspiofilemgr.h>
#include <psploadexec.h>
#include <psploadexec_kernel.h>
#include <psputility_modules.h>
#include <pspumd.h>
#include <module2.h>
#include <lflash0.h>
#include <macros.h>
#include <rebootconfig.h>
#include <systemctrl_se.h>
#include <string.h>
#include "offsets.h"
#include "functions.h"

/*
 * These Mappings are Syscall Mappings! Don't bother trying them inside
 * of a Kernel Permission Context! You will crash!
 */

// counter for relocated stubs
static u32 curcall = 0x08801000;

// fill FunctionTable
void getUserFunctions(FunctionTable* tbl){

	memset(tbl, 0, sizeof(FunctionTable));

	tbl->IoOpen = (void*)RelocSyscall("IoFileMgrForUser", 0x109F50BC);
	tbl->IoRead = (void*)RelocSyscall("IoFileMgrForUser", 0x6A638D83);
	tbl->IoClose = (void*)RelocSyscall("IoFileMgrForUser", 0x810C4BC3);

	tbl->IoWrite = (void*)RelocSyscall("IoFileMgrForUser", 0x42EC03AC);
	
	tbl->KernelLibcTime = (void*)RelocSyscall("UtilsForUser", 0x27CC57F0);
	tbl->KernelDcacheWritebackAll = (void*)RelocSyscall("UtilsForUser", 0x79D1C3FA);
	tbl->DisplaySetFrameBuf = (void*)RelocSyscall("sceDisplay", 0x289D82FE);

	tbl->KernelCreateThread = (void*)RelocSyscall("ThreadManForUser", 0x446D8DE6);
	tbl->KernelDelayThread = (void*)RelocSyscall("ThreadManForUser", 0xCEADEB47);
	tbl->KernelStartThread = (void*)RelocSyscall("ThreadManForUser", 0xF475845D);
	tbl->KernelWaitThreadEnd = (void*)RelocSyscall("ThreadManForUser", 0x278C0DF5);

	tbl->KernelDeleteVpl = (void*)RelocSyscall("ThreadManForUser", 0x89B3D48C);
	tbl->KernelDeleteFpl = (void*)RelocSyscall("ThreadManForUser", 0xED1410E0);
	
	tbl->UtilityLoadModule = (void*)RelocSyscall("sceUtility", 0x2A2B3DE0);
	tbl->UtilityUnloadModule = (void*)RelocSyscall("sceUtility", 0xE49BFE92);
	tbl->UtilityLoadNetModule = (void*)RelocSyscall("sceUtility", 0x1579A159);
	tbl->UtilityUnloadNetModule = (void*)RelocSyscall("sceUtility", 0x64D50C56);
	
	tbl->SysMemUserForUser_91DE343C = (void*)RelocSyscall("SysMemUserForUser", 0x91DE343C);
	tbl->KernelFreePartitionMemory = (void*)RelocSyscall("SysMemUserForUser", 0xB6D61D02);

	tbl->freeMem = &freeMem;
	tbl->FindImportUserRam = &FindImportUserRam;
	tbl->FindImportVolatileRam = &FindImportVolatileRam;
	tbl->FindImportRange = &FindImportRange;
	tbl->RelocSyscall = &RelocSyscall;
}

int IsUID(SceUID uid){
	return ((uid > 0 && uid < 0x05000000) && ((uid & 1) == 1));
}

// attempt to free as much memory as possible, some kernel exploits need this, others don't
void freeMem(FunctionTable* g_tbl){

/*
	u16 utilities[] = { PSP_MODULE_NET_ADHOC, PSP_MODULE_USB_PSPCM, PSP_MODULE_USB_MIC, PSP_MODULE_USB_CAM, \
		PSP_MODULE_USB_GPS, PSP_MODULE_AV_AVCODEC, PSP_MODULE_AV_SASCORE, PSP_MODULE_AV_ATRAC3PLUS, \
		PSP_MODULE_AV_MPEGBASE, PSP_MODULE_AV_MP3, PSP_MODULE_AV_VAUDIO, PSP_MODULE_AV_AAC, PSP_MODULE_AV_G729, \
		PSP_MODULE_NP_COMMON, PSP_MODULE_NP_SERVICE, PSP_MODULE_NP_MATCHING2, PSP_MODULE_NP_DRM
	};
	
	if (1){
	u32 i;
	for (i=0; i<sizeof(utilities)/sizeof(u16); i++)
		g_tbl->UtilityUnloadModule(utilities[i]);
	}
*/

	u32 (* sceKernelGetBlockHeadAddr_)(SceUID) = (void *)FindImportUserRam("SysMemUserForUser", 0x9D9A5BA1);
	int (* sceKernelDeleteFpl_)(SceUID) = (void *)FindImportUserRam("ThreadManForUser", 0xED1410E0);
	int (* sceKernelTerminateThread_)(SceUID) = (void *)FindImportUserRam("ThreadManForUser", 0x616403BA);
	int (* sceKernelSuspendThread_)(SceUID) = (void *)FindImportUserRam("ThreadManForUser", 0x9944F31F);

	u32 addr;

	for (addr = 0x08800000; addr < 0x0A000000; addr += 4) {
		if (IsUID(*(SceUID*)addr)) {
			if ((sceKernelGetBlockHeadAddr_) && (sceKernelGetBlockHeadAddr_(*(SceUID*)addr) > 0))
				g_tbl->KernelFreePartitionMemory(*(SceUID*)addr);

			if (sceKernelDeleteFpl_)
				sceKernelDeleteFpl_(*(SceUID*)addr);

			if (sceKernelTerminateThread_)
				sceKernelTerminateThread_(*(SceUID*)addr);
			else if (sceKernelSuspendThread_)
				sceKernelSuspendThread_(*(SceUID*)addr);
		}
	}
}

int AddressInRange(u32 addr, u32 lower, u32 higher){
	return (addr >= lower && addr < higher);
}

u32 FindImportRange(char *libname, u32 nid, u32 lower, u32 higher){
	u32 i;
	for(i = lower; i < higher; i += 4) {
		SceLibraryStubTable *stub = (SceLibraryStubTable *)i;

		if((stub->libname != libname) && AddressInRange((u32)stub->libname, lower, higher) \
			&& AddressInRange((u32)stub->nidtable, lower, higher) && AddressInRange((u32)stub->stubtable, lower, higher)) {
			if(strcmp(libname, stub->libname) == 0) {
				u32 *table = stub->nidtable;

				int j;
				for(j = 0; j < stub->stubcount; j++) {
					if(table[j] == nid) {
						return ((u32)stub->stubtable + (j * 8));
					}
				}
			}
		}
	}

	return 0;
}

u32 FindImportVolatileRam(char *libname, u32 nid){
	return FindImportRange(libname, nid, 0x08400000, 0x08800000);
}

u32 FindImportUserRam(char *libname, u32 nid){
	return FindImportRange(libname, nid, 0x08800000, 0x0A000000);
}

void *RelocSyscall(char* lib, u32 nid){

	u32 call = FindImportUserRam(lib, nid);
	
	return (void*)call;

    if (call != 0) {
        while (_lw(curcall))
            curcall += 8;

        *(u32*)curcall = *(u32*)call;
        *(u32*)(curcall + 4) = *(u32*)(call + 4);

        return (void *)curcall;
    }

    return 0;
}

/*
 * These functions are ment for using when initial kernel access has been
 * granted, for example through the mean of a kernel exploit.
 */


void getKernelFunctions(KernelFunctions* kfuncs){

	kfuncs->KernelIOOpen = (void*)FindFunction("sceIOFileManager", "IoFileMgrForKernel", 0x109F50BC);
	kfuncs->KernelIORead = (void*)FindFunction("sceIOFileManager", "IoFileMgrForKernel", 0x6A638D83);
	kfuncs->KernelIOClose = (void*)FindFunction("sceIOFileManager", "IoFileMgrForKernel", 0x810C4BC3);
	kfuncs->KernelIOWrite = (void*)FindFunction("sceIOFileManager", "IoFileMgrForKernel", 0x42EC03AC);
	kfuncs->KernelIOMkdir = (void*)FindFunction("sceIOFileManager", "IoFileMgrForKernel", 0x06A70004);
	kfuncs->KernelIORmdir = (void*)FindFunction("sceIOFileManager", "IoFileMgrForKernel", 0x1117C65F);
	kfuncs->KernelIODopen = (void*)FindFunction("sceIOFileManager", "IoFileMgrForKernel", 0xB29DDF9C);
	kfuncs->KernelIODread = (void*)FindFunction("sceIOFileManager", "IoFileMgrForKernel", 0xE3EB004C);
	kfuncs->KernelIODclose = (void*)FindFunction("sceIOFileManager", "IoFileMgrForKernel", 0xEB092469);
	kfuncs->KernelIOGetStat = (void*)FindFunction("sceIOFileManager", "IoFileMgrForKernel", 0xACE946E8);
	
	kfuncs->KernelIcacheInvalidateAll = (void*)FindFunction("sceSystemMemoryManager", "UtilsForKernel", 0x920F104A);
	kfuncs->KernelDcacheWritebackInvalidateAll = (void*)FindFunction("sceSystemMemoryManager", "UtilsForKernel", 0xB435DEC5);
	kfuncs->KernelDcacheInvalidateRange = (void*)FindFunction("sceSystemMemoryManager", "UtilsForKernel", 0xBFA98062);
	kfuncs->KernelGzipDecompress = (void*)FindFunction("sceSystemMemoryManager", "UtilsForKernel", 0x78934841);
	
	kfuncs->KernelCtrlSetSamplingCycle = (void*)FindFunction("sceController_Service", "sceCtrl", 0x6A2774F3);
	kfuncs->KernelCtrlSetSamplingMode = (void*)FindFunction("sceController_Service", "sceCtrl", 0x1F4011E6);
	kfuncs->KernelCtrlPeekBufferPositive = (void*)FindFunction("sceController_Service", "sceCtrl", 0x3A622550);
	
	kfuncs->Kermit_driver_4F75AA05 = (void*)FindFunction("sceKermit_Driver", "sceKermit_driver", 0x4F75AA05);
	
	kfuncs->KernelFindModuleByName = (void*)FindFunction("sceLoaderCore", "LoadCoreForKernel", 0xF6B1BF0F);
	
	kfuncs->KernelCreateThread = (void*)FindFunction("sceThreadManager", "ThreadManForKernel", 0x446D8DE6);
	kfuncs->KernelStartThread = (void*)FindFunction("sceThreadManager", "ThreadManForKernel", 0xF475845D);
	kfuncs->KernelDelayThread = (void*)FindFunction("sceThreadManager", "ThreadManForKernel", 0xCEADEB47);
}

u32 FindTextAddrByName(const char *modulename)
{
	u32 kaddr;
	for (kaddr = 0x88000000; kaddr < 0x88400000; kaddr += 4) {
		if (strcmp((const char *)kaddr, modulename) == 0) {
			if ((*(u32*)(kaddr + 0x64) == *(u32*)(kaddr + 0x78)) && \
				(*(u32*)(kaddr + 0x68) == *(u32*)(kaddr + 0x88))) {
				if (*(u32*)(kaddr + 0x64) && *(u32*)(kaddr + 0x68))
					return *(u32*)(kaddr + 0x64);
			}
		}
	}
	return 0;
}

u32 FindFunction(const char *module, const char *library, u32 nid)
{
	u32 addr = FindTextAddrByName(module);

	if (addr) {
		u32 maxaddr = 0x88400000;

		if (addr >= 0x08800000 && addr < 0x0A000000)
			maxaddr = 0x0A000000;
		else if (addr >= 0x08400000 && addr < 0x08800000)
			maxaddr = 0x08800000;

		for (; addr < maxaddr; addr += 4) {
			if (strcmp(library, (const char *)addr) == 0) {
				u32 libaddr = addr;

				while (*(u32*)(addr -= 4) != libaddr);

				u32 exports = (u32)(*(u16*)(addr + 10) + *(u8*)(addr + 9));
				u32 jump = exports * 4;

				addr = *(u32*)(addr + 12);

				while (exports--) {
					if (*(u32*)addr == nid)
						return *(u32*)(addr + jump);

					addr += 4;
				}

				return 0;
			}
		}
	}

	return 0;
}

u32 _findJAL(u32 addr, int reversed, int skip){
	int add = 4;
	if (reversed)
		add = -4;
	for(;;addr+=add) {
		if ((_lw(addr) >= 0x0C000000) && (_lw(addr) < 0x10000000)){
			if (skip == 0)
				return (((_lw(addr) & 0x03FFFFFF) << 2) | 0x80000000);
			else
				skip--;
		}
	}

	return 0;
}

u32 findFirstJAL(u32 addr){
	return _findJAL(addr, 0, 0);
}

u32 findJAL(u32 addr, int pos){
	return _findJAL(addr, 0, pos);
}

u32 findFirstJALReverse(u32 addr){
	return  _findJAL(addr, 1, 0);
}

u32 findJALReverse(u32 addr, int pos){
	return  _findJAL(addr, 1, pos);
}

u32 findFirstJALForFunction(char* modname, char* libname, u32 uid){
	u32 somefunc = FindFunction(modname, libname, uid);
	return findFirstJAL(somefunc);
}

u32 findJALForFunction(char* modname, char* libname, u32 uid, int pos){
	u32 somefunc = FindFunction(modname, libname, uid);
	return findJAL(somefunc, pos);
}

u32 findFirstJALReverseForFunction(char* modname, char* libname, u32 uid){
	u32 somefunc = FindFunction(modname, libname, uid);
	return findFirstJALReverse(somefunc);
}

u32 findJALReverseForFunction(char* modname, char* libname, u32 uid, int pos){
	u32 somefunc = FindFunction(modname, libname, uid);
	return findJALReverse(somefunc, pos);
}

u32 findRefInGlobals(char* libname, u32 addr, u32 ptr){
	while (strcmp(libname, (char*)addr))
		addr++;
	
	if (addr%4)
		addr &= -0x4; // align to 4 bytes

	while (_lw(addr += 4) != ptr);
	
	return addr;
}
