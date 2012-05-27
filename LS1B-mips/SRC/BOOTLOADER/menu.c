//
// Copyright (c) Microsoft Corporation.  All rights reserved.
//
//
// Use of this sample source code is subject to the terms of the Microsoft
// license agreement under which you licensed this sample source code. If
// you did not accept the terms of the license agreement, you are not
// authorized to use this sample source code. For the terms of the license,
// please see the license agreement between you and Microsoft or, if applicable,
// see the LICENSE.RTF on your install media or the root of your tools installation.
// THE SAMPLE SOURCE CODE IS PROVIDED "AS IS", WITH NO WARRANTIES.
//
//------------------------------------------------------------------------------
//
//  File: menu.c
//
//  This file implements platform dependend boot loader menu.
//
#include <boot.h>
#include <oal_blmenu.h>

//------------------------------------------------------------------------------
// External Variables 

extern UINT32 g_bootPage;

//------------------------------------------------------------------------------
// Local Functions

VOID ShowSettings(OAL_BLMENU_ITEM *pMenu);
VOID ShowNetworkSettings(OAL_BLMENU_ITEM *pMenu);
VOID SaveSettings(OAL_BLMENU_ITEM *pMenu);

//------------------------------------------------------------------------------
// Local Variables 

static OAL_BLMENU_ITEM g_menuNetwork[] = {
    { 
        L'1', L"Show Current Settings", ShowNetworkSettings,
        NULL, NULL, NULL
    }, { 
        L'2', L"Enable/disable DHCP", OALBLMenuEnable,
        L"DHCP", &g_bootCfg.kitlFlags, (VOID*)OAL_KITL_FLAGS_DHCP
    }, {
        L'3', L"Set IP address", OALBLMenuSetIpAddress,
        L"Device", &g_bootCfg.ipAddress, NULL
    }, { 
        L'4', L"Set IP mask", OALBLMenuSetIpMask,
        L"Device", &g_bootCfg.ipMask, NULL
    }, { 
        L'5', L"Set default router", OALBLMenuSetIpAddress,
        L"Default router", &g_bootCfg.ipRoute, NULL
    }, { 
        L'6', L"Enable/disable VMINI", OALBLMenuEnable,
        L"VMINI", &g_bootCfg.kitlFlags, (VOID*)OAL_KITL_FLAGS_VMINI
    }, { 
        L'0', L"Exit and Continue", NULL,
        NULL, NULL, NULL
    }, {
        0, NULL, NULL,
        NULL, NULL, NULL
    }
};

static OAL_BLMENU_ITEM g_menuMain[] = {
    { 
        L'1', L"Show Current Settings", ShowSettings,
        NULL, NULL, NULL
    }, { 
        L'2', L"Set Device Id", OALBLMenuSetDeviceId,
        g_bootCfg.deviceId, NULL, NULL
    }, { 
        L'3', L"Select Boot Device", OALBLMenuSelectDevice,
        L"Boot", &g_bootCfg.bootDevLoc, g_bootDevices
    }, {
        L'4', L"Select Debug Device", OALBLMenuSelectDevice,
        L"Debug", &g_bootCfg.kitlDevLoc, g_kitlDevices
    }, { 
        L'5', L"Network Settings", OALBLMenuShow,
        L"Network Settings", &g_menuNetwork, NULL
    }, { 
        L'6', L"Save Settings", SaveSettings,
        NULL, NULL, NULL
    }, { 
        L'0', L"Exit and Continue", NULL,
        NULL, NULL, NULL
    }, {
        0, NULL, NULL,
        NULL, NULL, NULL
    }
};

static OAL_BLMENU_ITEM g_menuRoot = {
    0, NULL, OALBLMenuShow,
    L"Main Menu", g_menuMain, NULL
};

//------------------------------------------------------------------------------

VOID BLMenu()
{
    // Simply activate main menu
    OALBLMenuActivate(5, &g_menuRoot);
}

//------------------------------------------------------------------------------

VOID ShowSettings(OAL_BLMENU_ITEM *pMenu)
{
    OALLog(
        L" Device Id:     %S\r\n", 
        (g_bootCfg.deviceId[0] == '\0') ? "*** auto ***" : g_bootCfg.deviceId
    );
    OALLog(
        L" Boot device:   %s\r\n", 
        OALKitlDeviceName(&g_bootCfg.bootDevLoc, g_bootDevices)
    );
    OALLog(
        L" Debug device:  %s\r\n", 
        OALKitlDeviceName(&g_bootCfg.kitlDevLoc, g_kitlDevices)
    );
    OALLog(
        L" DHCP:          %s\r\n", 
        (g_bootCfg.kitlFlags & OAL_KITL_FLAGS_DHCP) ? L"enabled" : L"disabled"
    );
    OALLog(L" IP address:    %s\r\n", OALKitlIPtoString(g_bootCfg.ipAddress));
    OALLog(L" IP mask:       %s\r\n", OALKitlIPtoString(g_bootCfg.ipMask));
    OALLog(L" IP router:     %s\r\n", OALKitlIPtoString(g_bootCfg.ipRoute));
    OALLog(
        L" VMINI:         %s\r\n", 
        (g_bootCfg.kitlFlags & OAL_KITL_FLAGS_VMINI) ? L"enabled" : L"disabled"
    );
}

//------------------------------------------------------------------------------

VOID ShowNetworkSettings(OAL_BLMENU_ITEM *pMenu)
{
    OALLog(
        L" DHCP:          %s\r\n", 
        (g_bootCfg.kitlFlags & OAL_KITL_FLAGS_DHCP) ? L"enabled" : L"disabled"
    );
    OALLog(L" IP address:    %s\r\n", OALKitlIPtoString(g_bootCfg.ipAddress));
    OALLog(L" IP mask:       %s\r\n", OALKitlIPtoString(g_bootCfg.ipMask));
    OALLog(L" IP router:     %s\r\n", OALKitlIPtoString(g_bootCfg.ipRoute));
    OALLog(
        L" VMINI:         %s\r\n", 
        (g_bootCfg.kitlFlags & OAL_KITL_FLAGS_VMINI) ? L"enabled" : L"disabled"
    );
}

//------------------------------------------------------------------------------

VOID SaveSettings(OAL_BLMENU_ITEM *pMenu)
{
    WCHAR key;
    UINT32 config;
        
    OALLog(L" Do you want save current settings [-/y]? ");
    
    // Get key
    key = OALBLMenuReadKey(TRUE);
    OALLog(L"%c\r\n", key);
    
    // Depending on result
    if (key != L'y' && key != L'Y') goto cleanUp;

    config  = IMAGE_FLASH_PA_BOOT_START + IMAGE_FLASH_BOOT_SIZE * g_bootPage;
    config += IMAGE_BOOT_CONFIG_PA_START - IMAGE_BOOT_FLASH_PA_START;

    // Erase configuration page
    if (!OALFlashErase(
        OALPAtoUA(IMAGE_FLASH_PA_START), OALPAtoUA(config), 
        IMAGE_BOOT_CONFIG_SIZE
    )) {
        OALLog(L" Flash memory erase failed\r\n");
        goto cleanUp;
    }

    // Write configuration
    if (!OALFlashWrite(
        OALPAtoUA(IMAGE_FLASH_PA_START), OALPAtoUA(config), sizeof(g_bootCfg),
        &g_bootCfg
    )) {
        OALLog(L" Flash memory write failed\r\n");
        goto cleanUp;
    }

    OALLog(L" Current settings has been saved\r\n");
    
cleanUp: ;
}

//------------------------------------------------------------------------------

