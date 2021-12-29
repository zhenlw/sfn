#pragma once

#include "resource.h"

struct kmap_t {
    DWORD vkt, flags, scode;
    int in_mapped_state; //the key is kept mapped so long as it is pressed, no matter whether the act key is released.
};

extern DWORD g_act_vk;
extern kmap_t g_kmap[256];

void load_config();

void InstallHook();
void RemoveHook();
//bool IsEnabled();
bool ShouldShowInNotificationArea();
void ReHook();
//void CheckForUpdate();
