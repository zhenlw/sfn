#include <iostream>
#include <windows.h>

//configurations
static DWORD c_act_vk = VK_SPACE;

struct kmap_t {
    DWORD vko, vkt, flags;
    int in_mapped_state; //the key is kept mapped so long as it is pressed, no matter whether the act key is released.
};

static HHOOK HookHandle = NULL;
static int s_iState = 0;
static kmap_t s_kmap[256];
const ULONG_PTR injectedFlag = 'TCUR';

static void load_config() {
    memset(s_kmap, 0, sizeof(s_kmap));
    c_act_vk = VK_SPACE;
    s_kmap['I'].vkt = VK_UP;
    s_kmap['J'].vkt = VK_LEFT;
    s_kmap['K'].vkt = VK_DOWN;
    s_kmap['L'].vkt = VK_RIGHT;
    s_kmap['U'].vkt = VK_HOME;
    s_kmap['O'].vkt = VK_END;
    s_kmap['H'].vkt = VK_PRIOR;
    s_kmap['N'].vkt = VK_NEXT;
    s_kmap[VK_OEM_1].vkt = VK_DELETE;
    s_kmap['B'].vkt = VK_SPACE;

    /*const DWORD extended[] = {
      VK_LEFT, VK_RIGHT, VK_UP, VK_DOWN,
      VK_HOME, VK_END, VK_PRIOR, VK_NEXT, VK_SNAPSHOT,
      VK_INSERT, VK_DELETE, VK_DIVIDE, VK_NUMLOCK,
      VK_RCONTROL, VK_RMENU,
      0
      };
      for (int i = 0; i < 256; i++) {
      if (s_kmap[i].vkt > 0) {
      for (const DWORD* p = extended; *p; ++p) {
      if (s_kmap[i].vkt == *p) s_kmap[i].flags = KEYEVENTF_EXTENDEDKEY;
      }
      }
      }*/
}

static void sendKey(WORD code, DWORD flags)
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
	if (h->vkCode == c_act_vk) {
	    if (s_iState < 2) s_iState++; //on repeat the state will be 2 like with the "other keys presses case below, so we don't send the act key tap on up too, if this happens
	    return 1;
	}

	if (s_iState != 0) {
	    //key down case in activated mode
	    s_iState = 2; //so we don't send the act key tap on up
	    if (s_kmap[h->vkCode].vkt >0) {
		//std::cout << "vkt " << s_kmap[h->vkCode].vkt;
		sendKey(s_kmap[h->vkCode].vkt, s_kmap[h->vkCode].flags);
		s_kmap[h->vkCode].in_mapped_state = 1;
		return 1;
	    }
	}
	//just go through default routine
    }
    else {
	if (h->vkCode == c_act_vk) {
	    //act key up, send a tap and clean up state
	    if (s_iState == 1) {
		sendKey(c_act_vk, s_kmap[h->vkCode].flags);
		sendKey(c_act_vk, s_kmap[h->vkCode].flags | KEYEVENTF_KEYUP);
	    }
	    s_iState = 0;
	    return 1;
	}
	//whether in act mode, we still look the key up in the list to see what kc to send
	if (s_kmap[h->vkCode].in_mapped_state) {
	    sendKey(s_kmap[h->vkCode].vkt, s_kmap[h->vkCode].flags | KEYEVENTF_KEYUP);
	    s_kmap[h->vkCode].in_mapped_state = 0;
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

void LoadOptions()
{
}

void ReHook()
{
}
