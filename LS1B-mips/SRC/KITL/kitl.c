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
//  File: kitl.c
//
#include <bsp.h>
#include <kitl_cfg.h>

//------------------------------------------------------------------------------

BOOL OEMKitlStartup (void)
{
    BOOL rc = FALSE;
    OAL_KITL_ARGS *pArgs, args;
    CHAR *szDeviceId;

    KITL_RETAILMSG(ZONE_KITL_OAL, ("+OALKitlStart\r\n"));

    // First get all boot args
    pArgs = OALArgsQuery(OAL_ARGS_QUERY_KITL);
    szDeviceId = OALArgsQuery(OAL_ARGS_QUERY_DEVID);

    // If we don't get kitl arguments use default
    if (pArgs == NULL) {
        
        KITL_RETAILMSG(ZONE_WARNING, ("WARN: Boot arguments not found, use defaults\r\n"));
        memset(&args, 0, sizeof(args));
        args.flags = OAL_KITL_FLAGS_ENABLED|OAL_KITL_FLAGS_DHCP|OAL_KITL_FLAGS_VMINI;
        args.ipAddress = 0;
        args.devLoc.IfcType = PCIBus;
        args.devLoc.BusNumber = 0;
        args.devLoc.LogicalLoc =  0x00010100;
        args.devLoc.Pin = 1;
        pArgs = &args;
    }        

    // If there isn't device id from bootloader let KITL create some for us
    if (szDeviceId == NULL || szDeviceId[0] == '\0') {
        szDeviceId = BSP_DEVICE_PREFIX;
        pArgs->flags |= OAL_KITL_FLAGS_EXTNAME;
    }

    // We need configure device bus
    switch (pArgs->devLoc.IfcType) {
    case PCIBus:
        if (!OALPCIConfig(
            0, BSP_REG_PA_PCI_MEMOFFS, BSP_REG_PCI_MEMSIZE, 
            BSP_REG_PA_PCI_IOOFFS + 0x1000, BSP_REG_PCI_IOSIZE, 1, 
            (OAL_PCI_LOCATION*)&pArgs->devLoc.LogicalLoc
        )) {
           KITL_RETAILMSG(ZONE_ERROR, ("ERROR: PCI Bus configuration failed\r\n"));
           goto cleanUp;
        }
        break;
    }

    // Finally call KITL library
    rc = OALKitlInit(szDeviceId, pArgs, g_kitlDevices);

cleanUp:
    KITL_RETAILMSG(ZONE_KITL_OAL, ("-OALKitlStart(rc = %d)\r\n", rc));
    return rc;
}

#define COUNTS_PER_MS   (BSP_CP0_COUNTFREQ/1000)

//------------------------------------------------------------------------------
//
//  External function:  OALTimerGetCount
//
//  This function is implemented in MIPS CP0 library. It returns actual CP0
//  count register value.
//
UINT32 OALTimerGetCount();

//------------------------------------------------------------------------------
//
//  Function:  OALStall
//
//  Wait for time specified in parameter in microseconds (busy wait). This
//  function can be called in hardware/kernel initialization process.
//
VOID OALStall(UINT32 microSec)
{
    UINT32 base, counts;

    while (microSec > 0) {
        if (microSec > 1000) {
            counts = COUNTS_PER_MS;
            microSec -= 1000;
        } else {
            counts = (microSec * COUNTS_PER_MS)/1000;
            microSec = 0;
        }            
        base = OALTimerGetCount();
        while ((OALTimerGetCount() - base) < counts);
    }
}

//------------------------------------------------------------------------------
//
//  Function:  OALGetTickCount
//
//  Function returns number of 1ms ticks since system reboot/reset.
//
UINT32 OALGetTickCount()
{
    static UINT32 base = 0, offset = 0;
    UINT32 plus, delta;

    delta = OALTimerGetCount() - base;
    plus = delta/COUNTS_PER_MS;
    offset += plus;
    base += plus * COUNTS_PER_MS;
    return offset;
}

//------------------------------------------------------------------------------

//------------------------------------------------------------------------------
