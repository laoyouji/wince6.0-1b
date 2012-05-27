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
//  File:  boot_cfg.h
//
#ifndef __BOOT_CFG_H
#define __BOOT_CFG_H

//------------------------------------------------------------------------------

OAL_KITL_DEVICE g_bootDevices[] = {
    { 
        L"AMD Am79C973", PCIBus, 0x20001022, 1, OAL_KITL_TYPE_ETH,
        &g_kitlEthAM79C973
    }, {
        L"RTL RTL8139 ", PCIBus, 0x12111113, 0, OAL_KITL_TYPE_ETH,
        &g_kitlEthRTL8139
    }, {
        L"RTL RTL8139 ", PCIBus, 0x813910ec, 0, OAL_KITL_TYPE_ETH,
        &g_kitlEthRTL8139
    }, {
        L"Flash Memory", 
        Internal, IMAGE_FLASH_PA_START, 0, OAL_KITL_TYPE_FLASH, NULL
    }, {
        NULL, 0, 0, 0, 0, NULL
    }
};    

//------------------------------------------------------------------------------

#endif
