#include <iostream>
#include <windows.h>
#include "sfn.h"

DWORD g_act_vk = VK_SPACE;
kmap_t g_kmap[256];

static HHOOK HookHandle = NULL;
static int s_iState = 0;
const ULONG_PTR injectedFlag = 'TCUR';


static void sendKey(WORD code, DWORD scode, DWORD flags)
{
    //if (up) flags |= KEYEVENTF_KEYUP;
    //if (isExtendedKey(code)) flags |= KEYEVENTF_EXTENDEDKEY;

    keybd_event(code, MapVirtualKey(code, 0), flags, injectedFlag);
}

static LRESULT CALLBACK LowLevelKeyboardProc(int nCode, WPARAM wParam, LPARAM lParam)
{
    if ( nCode < 0 )
	return CallNextHookEx(NULL, nCode, wParam, lParam);

    KBDLLHOOKSTRUCT* h = reinterpret_cast<KBDLLHOOKSTRUCT*>(lParam);
    if (h->flags & LLKHF_INJECTED && h->dwExtraInfo == injectedFlag)
	return CallNextHookEx(NULL, nCode, wParam, lParam);

    //std::cout << "codes " << h->vkCode << " state " << s_iState << std::endl;
    if (wParam == WM_KEYDOWN || wParam == WM_SYSKEYDOWN) {
	//std::cout << "down" << std::endl;
	if (h->vkCode == g_act_vk) {
	    if (s_iState < 2) s_iState++; //on repeat the state will be 2 like with the "other keys presses case below, so we don't send the act key tap on up too, if this happens
	    return 1;
	}

	if (s_iState != 0) {
	    //key down case in activated mode
	    s_iState = 2; //so we don't send the act key tap on up
	    if (g_kmap[h->vkCode].vkt >0) {
		//std::cout << "vkt " << g_kmap[h->vkCode].vkt;
		sendKey(g_kmap[h->vkCode].vkt, g_kmap[h->vkCode].scode, g_kmap[h->vkCode].flags);
		g_kmap[h->vkCode].in_mapped_state = 1;
		return 1;
	    }
	}
	//just go through default routine
    }
    else {
	if (h->vkCode == g_act_vk) {
	    //act key up, send a tap and clean up state
	    if (s_iState == 1) {
		sendKey(g_act_vk, g_kmap[h->vkCode].scode, g_kmap[h->vkCode].flags);
		sendKey(g_act_vk, g_kmap[h->vkCode].scode, g_kmap[h->vkCode].flags | KEYEVENTF_KEYUP);
	    }
	    s_iState = 0;
	    return 1;
	}
	//whether in act mode, we still look the key up in the list to see what kc to send
	if (g_kmap[h->vkCode].in_mapped_state) {
	    sendKey(g_kmap[h->vkCode].vkt, g_kmap[h->vkCode].scode, g_kmap[h->vkCode].flags | KEYEVENTF_KEYUP);
	    g_kmap[h->vkCode].in_mapped_state = 0;
	    return 1;
	}
	//otherwise do nothing more than rutine process
    }
    return CallNextHookEx(NULL, nCode, wParam, lParam);
}

void InstallHook()
{
    load_config();
    HookHandle = SetWindowsHookEx(WH_KEYBOARD_LL, LowLevelKeyboardProc, NULL, 0);
    SetThreadPriority(GetCurrentThread(), THREAD_PRIORITY_TIME_CRITICAL);
}

void RemoveHook()
{
    UnhookWindowsHookEx(HookHandle);
    HookHandle = NULL;
}

void ReHook()
{
}
