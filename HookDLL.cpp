// HookDLL.cpp : Defines the entry point for the DLL application.
//
#include "stdafx.h"
#include "HookDLL.h"

#pragma data_seg(".HookDllSegment")
HWND hWndServer = NULL;
UINT UWM_HCBT_CREATEWND_ID = 0;
UINT UWM_HCBT_ACTIVATEWND_ID = 0;
UINT UWM_HCBT_SYSCOMMAND_ID = 0;
UINT UWM_HSHELL_WINDOWCREATED_ID = 0;
UINT UWM_HSHELL_REDRAW_ID = 0;
UINT UWM_HSHELL_WINDOWREPLACED_ID = 0;
Hook_Info cbt_hook = {};
Hook_Info shell_hook = {};
#pragma data_seg()
#pragma comment(linker, "/section:.HookDllSegment,rws")

//Dll instance 
HINSTANCE hInst;

static LRESULT CALLBACK CBTMsgProc(UINT nCode, WPARAM wParam, LPARAM lParam);
static LRESULT CALLBACK ShellMsgProc(UINT nCode, WPARAM wParam, LPARAM lParam);

BOOL APIENTRY DllMain(HINSTANCE hModule,
	DWORD  dwReason,
	LPVOID lpReserved)
{
	std::wstring lpcwTemp;
	switch (dwReason)
	{
	case DLL_PROCESS_ATTACH:
		hInst = hModule;
		lpcwTemp = s2ws(UWM_HCBT_CREATEWND);
		UWM_HCBT_CREATEWND_ID = ::RegisterWindowMessage(lpcwTemp.c_str());

		lpcwTemp = s2ws(UWM_HSHELL_WINDOWCREATED);
		UWM_HSHELL_WINDOWCREATED_ID = ::RegisterWindowMessage(lpcwTemp.c_str());

		/*lpcwTemp = s2ws(UWM_HCBT_ACTIVATE);
		UWM_HCBT_ACTIVATEWND_ID = ::RegisterWindowMessage(lpcwTemp.c_str());

		lpcwTemp = s2ws(UWM_HCBT_SYSCOMMAND);
		UWM_HCBT_SYSCOMMAND_ID = ::RegisterWindowMessage(lpcwTemp.c_str());		

		lpcwTemp = s2ws(UWM_HSHELL_REDRAW);
		UWM_HSHELL_REDRAW_ID = ::RegisterWindowMessage(lpcwTemp.c_str());

		lpcwTemp = s2ws(UWM_HSHELL_WINDOWREPLACED);
		UWM_HSHELL_WINDOWREPLACED_ID = ::RegisterWindowMessage(lpcwTemp.c_str());*/
		break;
		//**********************************************
		// PROCESS_DETACH
		//**********************************************
	case DLL_PROCESS_DETACH:
	default:
		break;
	}
	return TRUE;
}

__declspec(dllexport) BOOL InstallHook(HWND hWndParent, HookId* hookInstallID)
{
	if (hWndServer != NULL && hWndServer != hWndParent)
		return FALSE;

	if (*hookInstallID > UWM_WH_MINHOOK && *hookInstallID < UWM_WH_MAXHOOK)
	{
		switch (*hookInstallID)
		{
		case HookId::wh_cbt:
			if (HookNotInstalled(cbt_hook))
			{
				cbt_hook.hookID = HookId::wh_cbt;
				cbt_hook.hook = SetWindowsHookEx(HookId::wh_cbt - 1, (HOOKPROC)CBTMsgProc,
					hInst, 0);

				if (cbt_hook.hook != NULL)
				{
					hWndServer = hWndParent;
					return TRUE;
				}
			}
			break;
		case HookId::wh_shell:
			if (HookNotInstalled(shell_hook))
			{
				shell_hook.hookID = HookId::wh_shell;
				shell_hook.hook = SetWindowsHookEx(HookId::wh_shell - 1, (HOOKPROC)ShellMsgProc,
					hInst, 0);

				if (shell_hook.hook != NULL)
				{
					hWndServer = hWndParent;
					return TRUE;
				}
			}
			break;
		default:
			break;
		}
	}

	return FALSE;
}

__declspec(dllexport) BOOL UnInstallHook(HWND hWndParent, HookId* hookInstallID)
{
	BOOL unhooked = FALSE;

	switch (*hookInstallID)
	{
	case HookId::wh_cbt:
		if (!HookNotInstalled(cbt_hook))
			unhooked = UnhookWindowsHookEx(cbt_hook.hook);
		if (unhooked)
		{
			cbt_hook.hook = NULL;
		}
		break;
	case HookId::wh_shell:
		if (!HookNotInstalled(shell_hook))
			unhooked = UnhookWindowsHookEx(shell_hook.hook);
		if (unhooked)
		{
			shell_hook.hook = NULL;
		}
		break;
	default:
		break;
	}

	if (HookNotInstalled(cbt_hook) && HookNotInstalled(shell_hook))
		hWndServer = NULL;

	return unhooked;
}

__declspec(dllexport) INT UnInstallAllHooks()
{
	INT unhooked = 0;

	if (!HookNotInstalled(cbt_hook))
	{
		if (UnhookWindowsHookEx(cbt_hook.hook))
		{
			cbt_hook.hook = NULL;
		}
		else
		{
			unhooked = 1;
		}
	}

	if (!HookNotInstalled(shell_hook))
	{
		if (UnhookWindowsHookEx(shell_hook.hook))
		{
			shell_hook.hook = NULL;
		}
		else
		{
			unhooked = 2;
		}
	}

	if (HookNotInstalled(cbt_hook) && HookNotInstalled(shell_hook))
		hWndServer = NULL;

	return unhooked;
}

static LRESULT CALLBACK CBTMsgProc(UINT nCode, WPARAM wParam, LPARAM lParam)
{
	if (nCode < 0)
	{
		CallNextHookEx(cbt_hook.hook, nCode, wParam, lParam);
		return 0;
	}
	switch (nCode)
	{
		/*case HCBT_MOVESIZE:
			PostMessage(hWndServer, UWM_HCBT_MOVESIZE, wParam, lParam);
			break;
		case HCBT_MINMAX:
			PostMessage(hWndServer, UWM_HCBT_MINMAX, wParam, lParam);
			break;
		case HCBT_QS:
			PostMessage(hWndServer, UWM_HCBT_DESTROYWND_ID, wParam, lParam);
			break;*/
	case HCBT_CREATEWND:
		PostMessage(hWndServer, UWM_HCBT_CREATEWND_ID, wParam, lParam);
		break;
		/*case HCBT_DESTROYWND:
			PostMessage(hWndServer, UWM_HCBT_DESTROYWND_ID, wParam, lParam);
			break;
	case HCBT_ACTIVATE:
		PostMessage(hWndServer, UWM_HCBT_ACTIVATEWND_ID, wParam, lParam);
		break;
	case HCBT_CLICKSKIPPED:
		PostMessage(hWndServer, UWM_HCBT_DESTROYWND_ID, wParam, lParam);
		break;
	case HCBT_KEYSKIPPED:
		PostMessage(hWndServer, UWM_HCBT_DESTROYWND_ID, wParam, lParam);
		break;
	case HCBT_SYSCOMMAND:
		PostMessage(hWndServer, UWM_HCBT_SYSCOMMAND_ID, wParam, lParam);
		break;
	case HCBT_SETFOCUS:
		PostMessage(hWndServer, UWM_HCBT_DESTROYWND_ID, wParam, lParam);
		break;*/
	default:
		break;
	}
	return CallNextHookEx(cbt_hook.hook, nCode, wParam, lParam);
}

static LRESULT CALLBACK ShellMsgProc(UINT nCode, WPARAM wParam, LPARAM lParam)
{
	if (nCode < 0)
	{
		CallNextHookEx(shell_hook.hook, nCode, wParam, lParam);
		return 0;
	}
	switch (nCode)
	{
	case HSHELL_WINDOWCREATED:
		PostMessage(hWndServer, UWM_HSHELL_WINDOWCREATED_ID, wParam, lParam);
		break;
	/*case HSHELL_WINDOWDESTROYED:
		PostMessage(hWndServer, UWM_HSHELL_WINDOWDESTROYED_ID, wParam, lParam);
		break;
	case HSHELL_WINDOWREPLACED:
		PostMessage(hWndServer, UWM_HSHELL_WINDOWREPLACED_ID, wParam, lParam);
		break;*/
#if(WINVER >= 0x0400)
	/*case HSHELL_WINDOWACTIVATED:
		PostMessage(hWndServer, UWM_HSHELL_WINDOWCREATED_ID, wParam, lParam);
		break;
	case HSHELL_GETMINRECT:
		PostMessage(hWndServer, UWM_HSHELL_WINDOWCREATED_ID, wParam, lParam);
		break;
	case HSHELL_REDRAW:
		PostMessage(hWndServer, UWM_HSHELL_REDRAW_ID, wParam, lParam);
		break;
	case HSHELL_TASKMAN:
		PostMessage(hWndServer, UWM_HSHELL_WINDOWCREATED_ID, wParam, lParam);
		break;
	case HSHELL_LANGUAGE:
		PostMessage(hWndServer, UWM_HSHELL_WINDOWCREATED_ID, wParam, lParam);
		break;
	case HSHELL_SYSMENU:
		PostMessage(hWndServer, UWM_HSHELL_WINDOWCREATED_ID, wParam, lParam);
		break;
	case HSHELL_ENDTASK:
		PostMessage(hWndServer, UWM_HSHELL_WINDOWCREATED_ID, wParam, lParam);
		break;*/
#endif
	default:
		break;
	}
	return CallNextHookEx(shell_hook.hook, nCode, wParam, lParam);
}

BOOL HookNotInstalled(Hook_Info hook)
{
	if (hook.hook != NULL && hook.hookID > 0) //Hook already installed
		return FALSE;

	return TRUE;
}

std::wstring s2ws(const std::string& s)
{
	std::wstring stemp;
	int len;
	int slength = (int)s.length() + 1;
	len = MultiByteToWideChar(CP_ACP, 0, s.c_str(), slength, 0, 0);
	wchar_t* buf = new wchar_t[len];
	MultiByteToWideChar(CP_ACP, 0, s.c_str(), slength, buf, len);
	std::wstring r(buf);
	delete[] buf;
	return r;
}