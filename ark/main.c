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

#include "main.h"
#include <loadexec_patch.h>
#include "libs/debug/graphics.h"
#include "flashpatch.h"
#include "reboot.h"

#include "210patch.h"

FunctionTable tbl;
FunctionTable *g_tbl = NULL;
KernelFunctions k_funcs;
KxploitFunctions kxf;
KxploitFunctions* kf = NULL;

char* savepath = (char*)0x08803000;

// Sony Reboot Buffer Loader
int (* _LoadReboot)(void *, unsigned int, void *, unsigned int) = NULL;

// LoadExecVSHWithApitype Direct Call
int (* _KernelLoadExecVSHWithApitype)(int, char *, struct SceKernelLoadExecVSHParam *, int) = NULL;

void initKxploitFile()
{
	char k_path[SAVE_PATH_SIZE];
	strcpy(k_path, savepath);
	strcat(k_path, "K.BIN");
	
	SceUID fd = g_tbl->IoOpen(k_path, PSP_O_RDONLY, 0);
	g_tbl->IoRead(fd, (void *)(KXPLOIT_LOADADDR|0x40000000), 0x4000);
	g_tbl->IoClose(fd);
	void (* kEntryPoint)(KxploitFunctions*) = (void*)KXPLOIT_LOADADDR;
	kf = &kxf;
	kEntryPoint(kf);
}

void freeUsrsbk(){

	//u32 (* KernelGetModuleIdByAddress)(u32 modAddr) = (void *)FindImportUserRam("ModuleMgrForUser", 0xD8B73127);

}

// Entry Point
int exploitEntry(char* arg0) __attribute__((section(".text.startup")));
int exploitEntry(char* arg0)
{

	// copy the path of the save
	strcpy(savepath, arg0);

	// Clear BSS Segment
	clearBSS();

	// init function table
	g_tbl = &tbl;
	getUserFunctions(g_tbl);

	// init screen
	initScreen(g_tbl->DisplaySetFrameBuf);

	UNUSED(size_rebootbuffer);
	
	// Output Exploit Reach Screen
	PRTSTR("Starting");
	
	// read kxploit file into memory and initialize it
	initKxploitFile();
	
	if (kf->stubScanner(g_tbl) == 0)
	{
		// Corrupt Kernel (crashes here?)
		int ret = kf->doExploit();
		
		if (ret >= 0)
		{
			// Flush Cache
			g_tbl->KernelDcacheWritebackAll();

			// Refresh screen (vram buffer screws it)
			cls();
			
			// Output Loading Screen
			PRTSTR("Loading");
			
			// Trigger Kernel Permission Callback
			kf->executeKernel((u32)&kernelContentFunction);
		}
		else{
			PRTSTR("Exploit failed");
			_sw(0,0);
		}
	}
	else{
		PRTSTR("Scan failed");
		_sw(0, 0);
	}

	return 0;
}

int bootRecoveryCheck(char* recovery)
{
	
	// Allocate Buffer for Gamepad Data
	SceCtrlData data;
		
	// Set Sampling Cycle
	k_funcs.KernelCtrlSetSamplingCycle(0);
		
	// Set Sampling Mode (we don't need the analog stick really)
	k_funcs.KernelCtrlSetSamplingMode(PSP_CTRL_MODE_DIGITAL);
		
	// Poll 64 Times
	int i = 0; for(; i < 64; i++)
	{
		// Clear Memory
		memset(&data, 0, sizeof(data));
			
		// Poll Data
		k_funcs.KernelCtrlPeekBufferPositive(&data, 1);
			
		// Recovery Mode
		if((data.Buttons & PSP_CTRL_RTRIGGER) == PSP_CTRL_RTRIGGER)
		{
		
			SceIoStat stat;
				
			// Return Recovery Path
			if (k_funcs.KernelIOGetStat(recovery, &stat) >= 0)	return 1;
			else	break;
		}
	}
	
	// Normal Boot
	return 0;
}

void runMenu(void)
{
	PRTSTR("running menu");
	
	char path[SAVE_PATH_SIZE];
	memset(path, 0, SAVE_PATH_SIZE);
	strcpy(path, savepath);
	strcat(path, "VBOOT.PBP");

	char recoverypath[SAVE_PATH_SIZE];
	memset(recoverypath, 0, SAVE_PATH_SIZE);
	strcpy(recoverypath, savepath);
	strcat(recoverypath, "RECOVERY.PBP");
	
	// Prepare Homebrew Reboot
	char * ebootpath = (bootRecoveryCheck(recoverypath)) ? (recoverypath) : (path);
	struct SceKernelLoadExecVSHParam param;
	memset(&param, 0, sizeof(param));
	param.size = sizeof(param);
	param.args = strlen(ebootpath) + 1;
	param.argp = ebootpath;
	param.key = "game";

	// Trigger Reboot
	_KernelLoadExecVSHWithApitype(0x141, ebootpath, &param, 0x10000);
}

void findGameID(){

	SceUID fd = k_funcs.KernelIOOpen("disc0:/UMD_DATA.BIN", PSP_O_RDONLY, 0777);
	u8 skip;
	
	k_funcs.KernelIORead(fd, GAMEID, 4);
	k_funcs.KernelIORead(fd, &skip, 1);
	k_funcs.KernelIORead(fd, GAMEID+4, 5);
	
	k_funcs.KernelIOClose(fd);

}

int is210Firmware(){
	k_funcs.KernelIOMkdir("ms0:/PSP/GAME/ARKTESTFOLDER", 0777);
	return k_funcs.KernelIORmdir("ms0:/PSP/GAME/ARKTESTFOLDER");
}

// Kernel Permission Function
void kernelContentFunction(void)
{
	// Switch to Kernel Permission Level
	setK1Kernel();

	kf->repairInstruction();
	
	// backup savepath
	memset(SAVEPATH, 0, SAVE_PATH_SIZE);
	strcpy(SAVEPATH, savepath);
	
	// get kernel functions
	getKernelFunctions(&k_funcs);
	
	// detect firmware (<210 or >=210)
	NEWER_FIRMWARE = is210Firmware();
	
	// find game ID
	findGameID();

	// Find LoadExec Module
	SceModule2 * loadexec = k_funcs.KernelFindModuleByName("sceLoadExec");
	
	// Find Reboot Loader Function
	_LoadReboot = (void *)loadexec->text_addr;
	
	// Find LoadExec Functions
	_KernelLoadExecVSHWithApitype = (void *)findFirstJALForFunction("sceLoadExec", "LoadExecForKernel", 0xD8320A28);
	
	// make the common loadexec patches
	patchLoadExecCommon(loadexec, (u32)LoadReboot);

	if (NEWER_FIRMWARE){
		// Redirect KERMIT_CMD_ERROR_EXIT loadFlash function
		u32 swaddress = findFirstJALForFunction("sceKermitPeripheral_Driver", "sceKermitPeripheral_driver", 0x3943440D);
		_sw(JUMP(flashLoadPatch), swaddress);
		_sw(NOP, swaddress+4);
	}
	else{
		// Patch flash0 Filesystem Driver
		if(patchFlash0Archive() < 0)
		{
			return;
		}
	}

	// Invalidate Cache
	k_funcs.KernelIcacheInvalidateAll();
	k_funcs.KernelDcacheWritebackInvalidateAll();

	runMenu();

}

// Fake K1 Kernel Setting
void setK1Kernel(void)
{
	// Set K1 to Kernel Value
	__asm__ (
		"nop\n"
		"lui $k1,0x0\n"
	);
}

// Clear BSS Segment of Payload
void clearBSS(void)
{
	// BSS Start and End Address from Linkfile
	extern char __bss_start, __bss_end;
	
	// Clear Memory
	memset(&__bss_start, 0, &__bss_end - &__bss_start);
}
