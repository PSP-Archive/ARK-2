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

#ifndef _GAME_FUNCTIONS_H_
#define _GAME_FUNCTIONS_H_

#include <sdk.h>
#include <psploadexec_kernel.h>
#include "module2.h"
#include "offsets.h"

/*
 * These functions are usermode syscalls, not meant to be used from kernelmode.
 */
 
typedef struct FunctionTable
{
	int (* IoOpen)(char *, int, int);
	int (* IoRead)(int, void *, int);
	int (* IoWrite)(int, void *, int);
	int (* IoClose)(int);
	void (* KernelLibcTime)(int);
	SceUID (* KernelCreateThread)(const char *name, SceKernelThreadEntry entry, int initPriority, int stackSize, SceUInt attr, SceKernelThreadOptParam *option);
	int (* KernelStartThread)(SceUID thid, SceSize arglen, void *argp);
	void (* KernelDelayThread)(uint32_t);
	int (* KernelWaitThreadEnd)(SceUID thid, SceUInt *timeout);
	int (* UtilityLoadModule)(int);
	int (* UtilityUnloadModule)(int);
	int (* UtilityLoadNetModule)(int);
	int (* UtilityUnloadNetModule)(int);
	int (* SysMemUserForUser_91DE343C)( void *unk );
	int (* KernelFreePartitionMemory)(int);
	int (* KernelDeleteVpl)(int);
	int (* KernelDeleteFpl)(int);
	int (* DisplaySetFrameBuf)(void *topaddr, int bufferwidth, int pixelformat, int sync);
	void (* KernelDcacheWritebackAll)(void);
	// common ark functions
	void (* freeMem)(struct FunctionTable* g_tbl);
	u32 (* FindImportUserRam)(char *libname, u32 nid);
	u32 (* FindImportVolatileRam)(char *libname, u32 nid);
	u32 (* FindImportRange)(char *libname, u32 nid, u32 lower, u32 higher);
	void* (* RelocSyscall)(char* lib, u32 nid);
} FunctionTable;

// fills a FunctionTable instance with all available imports
extern void getUserFunctions(FunctionTable* tbl);

// check if a pointer is in a range
extern int AddressInRange(u32 addr, u32 lower, u32 higher);

// find an import within a given range
extern u32 FindImportRange(char *libname, u32 nid, u32 lower, u32 higher);
// find an import within volatile memory
extern u32 FindImportVolatileRam(char *libname, u32 nid);
// find an import within user ram
extern u32 FindImportUserRam(char *libname, u32 nid);

// relocate a syscall
extern void *RelocSyscall(char* lib, u32 nid);

// algorithm to free all possible memory
extern void freeMem(FunctionTable* g_tbl);

/*
 * These functions are ment for using when initial kernel access has been
 * granted, for example through the mean of a kernel exploit.
 */
 
typedef struct KernelFunctions{
	// iofilemgr.prx Functions
	SceUID (* KernelIOOpen)(const char *, int, int);
	int (* KernelIOWrite)(SceUID, const void *, unsigned);
	int (* KernelIORead)(SceUID, void *, unsigned);
	int (* KernelIOClose)(SceUID);
	SceUID (* KernelIODopen)(char *);
	int (* KernelIODread)(SceUID, SceIoDirent *);
	int (* KernelIODclose)(SceUID);
	int (* KernelIOMkdir)(const char*, SceMode);
	int (* KernelIORmdir)(const char* path);
	int (* KernelIOGetStat)(const char *file, SceIoStat *stat);

	// controller functions
	int (* KernelCtrlSetSamplingCycle)(int zero);
	int (* KernelCtrlSetSamplingMode)(int mode);
	int (* KernelCtrlPeekBufferPositive)(SceCtrlData * pad_data, int count);

	// sysmem.prx Functions
	void (* KernelIcacheInvalidateAll)(void);
	void (* KernelDcacheWritebackInvalidateAll)(void);
	int (* KernelGzipDecompress)(unsigned char *dest, unsigned int destSize, const unsigned char *src, void *unknown);
	void (* KernelDcacheInvalidateRange)(const void *p, unsigned int size);

	// loadcore.prx Functions
	SceModule2 * (* KernelFindModuleByName)(char *);

	// threadman.prx Functions
	SceUID (* KernelCreateThread)(const char *name, SceKernelThreadEntry entry,\
			int initPriority, int stackSize, SceUInt attr, SceKernelThreadOptParam *option);
	int (* KernelStartThread)(SceUID thid, SceSize arglen, void *argp);
	int (* KernelDelayThread)(int);


	int (* WlanGetEtherAddr)(unsigned char *destAddr);

	int (* Kermit_driver_4F75AA05)(KermitPacket *packet, u32 cmd_mode, u32 cmd, u32 argc, u32 allow_callback, u64 *resp);
	
	int (* KernelLoadExecVSHWithApitype)(int, char *, struct SceKernelLoadExecVSHParam *, int);
	
}KernelFunctions;

extern void getKernelFunctions(KernelFunctions* kfuncs);

extern u32 FindTextAddrByName(const char *);
extern u32 FindFunction(const char *modulename, const char *library, u32 nid);
extern u32 findFirstJAL(u32 addr);
extern u32 findFirstJALReverse(u32 addr);
extern u32 findJAL(u32 addr, int pos);
extern u32 findJALReverse(u32 addr, int pos);

extern u32 findFirstJALForFunction(char* modname, char* libname, u32 uid);
extern u32 findJALForFunction(char* modname, char* libname, u32 uid, int pos);

extern u32 findFirstJALReverseForFunction(char* modname, char* libname, u32 uid);
extern u32 findJALReverseForFunction(char* modname, char* libname, u32 uid, int pos);

extern u32 findRefInGlobals(char* libname, u32 addr, u32 ptr);

#endif
