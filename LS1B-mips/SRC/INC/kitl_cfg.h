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
//  File:  kitl_cfg.h
//
#ifndef __KITL_CFG_H
#define __KITL_CFG_H

//------------------------------------------------------------------------------

BOOL   VRC5477Uart1Init(KITL_SERIAL_INFO *pInfo);
UINT16 VRC5477Uart1Send(UINT8 *pData, UINT16 size);
UINT16 VRC5477Uart1Recv(UINT8 *pData, UINT16 size);
VOID   VRC5477Uart1EnableInts();
VOID   VRC5477Uart1DisableInts();

static OAL_KITL_SERIAL_DRIVER g_kitlSerialUart1 = {
    VRC5477Uart1Init,
    NULL,
    VRC5477Uart1Send,
    NULL,
    VRC5477Uart1Recv,
    VRC5477Uart1EnableInts,
    VRC5477Uart1DisableInts,
    NULL,
    NULL,
    NULL,       // pfnFlowControl set to NULL for now
};

//------------------------------------------------------------------------------

static OAL_KITL_ETH_DRIVER g_kitlEthAM79C973 = OAL_ETHDRV_AM79C973;
static OAL_KITL_ETH_DRIVER g_kitlEthRTL8139 = OAL_ETHDRV_RTL8139;

OAL_KITL_DEVICE g_kitlDevices[] = {
    { 
        L"AMD Am79C973 ", PCIBus, 0x20001022, 1, OAL_KITL_TYPE_ETH,
        &g_kitlEthAM79C973
    }, {
        L"RTL RTL8139  ", PCIBus, 0x12111113, 0, OAL_KITL_TYPE_ETH,
        &g_kitlEthRTL8139
    }, {
        L"RTL RTL8139  ", PCIBus, 0x813910ec, 0, OAL_KITL_TYPE_ETH,
        &g_kitlEthRTL8139
    }, {
        L"Vrc5477 UART1", Internal, VRC5477_REG_PA, 0, OAL_KITL_TYPE_SERIAL,
        &g_kitlSerialUart1
    }, {
        NULL, 0, 0, 0, 0, NULL
    }
};    

//------------------------------------------------------------------------------

#endif
