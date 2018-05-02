#ifndef _HOOK_DLL_H_
#define _HOOK_DLL_H_
#if _MSC_VER > 1000
#pragma once
#endif
#include <string>
#include "defines.h"
#include "defines_shared.cs.h"

/****************************************
Author: GOHC

Descriptiom:
HookID hookID : holds (WH_MINHOOK/WH_MAXHOOK) + 1. If WH_MSGFILTER hook is installed then the ID is 0
HWND hWndServer : HWND id of window to send messages to
HHOOK hook : the actual hook installed

****************************************/
typedef struct _Hook_Info
{
	HookId hookID;
	HWND hWndServer;
	HHOOK hook;
}Hook_Info;

extern "C" {
#define LIBSPEC __declspec(dllexport)
	LIBSPEC BOOL InstallHook(HWND hWndParent, HookId* hookInstallID);
	LIBSPEC BOOL UnInstallHook(HWND hWndParent, HookId* hookInstallID);
	LIBSPEC INT UnInstallAllHooks();
#undef LIBSPEC
};

///Return true if hook not installed false if installed
BOOL HookNotInstalled(Hook_Info hook);

std::wstring s2ws(const std::string& s);

#endif 
