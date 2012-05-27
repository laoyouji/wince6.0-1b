//
// Copyright (c) Microsoft Corporation.  All rights reserved.
//
//
// Use of this source code is subject to the terms of the Microsoft end-user
// license agreement (EULA) under which you licensed this SOFTWARE PRODUCT.
// If you did not accept the terms of the EULA, you are not authorized to use
// this source code. For a copy of the EULA, please see the LICENSE.RTF on your
// install media.
//
//------------------------------------------------------------------------------
//
//  File: init.c
//
#include <bsp.h>
#include <profiler.h>

void OALRTCInit(); // define in rtc.c

/*
    Global:  dwOEMDrWatsonSize

    Global variable which specify DrWatson buffer size. It can be fixed
    in config.bib via FIXUPVAR.
 */
#define DR_WATSON_SIZE_NOT_FIXEDUP (-1)
DWORD dwOEMDrWatsonSize = DR_WATSON_SIZE_NOT_FIXEDUP;


//------------------------------------------------------------------------------
//
//  Function:  OEMInit
//
//  This is Windows CE OAL initialization function. It is called from kernel
//  after basic initialization is made.
//
void OEMInit()
{
    // Set up the debug zones according to the fix-up variable initialOALLogZones
    OALLogSetZones(initialOALLogZones);

    OALMSG(OAL_FUNC, (L"+OEMInit\r\n"));

    // Set memory size for DrWatson kernel support
    dwNKDrWatsonSize = 0;
    if (dwOEMDrWatsonSize != DR_WATSON_SIZE_NOT_FIXEDUP) {
        dwNKDrWatsonSize = dwOEMDrWatsonSize;
    }

    // Update kernel variables
    g_pOemGlobal->pfnProfileTimerEnable = OEMProfileTimerEnable;
    g_pOemGlobal->pfnProfileTimerDisable = OEMProfileTimerDisable;

    // Initialize cache globals
    OALCacheGlobalsInit();

    // Initialize PCI bus, this is necessary only if boot loader
    // doesn't initialize it (older boot loaders)
    OALPCIInit();

    // Initialize RTC(before OALIntrInit to avoid unexpected interrupt)
    OALRTCInit();

    // Initialize interrupt module
    if (!OALIntrInit()) {
        OALMSG(OAL_ERROR, (
            L"ERROR: OEMInit: failed to initialize interrupts\r\n"
        ));
        goto cleanUp;
    }

    // Initialize system clock
    if (!OALTimerInit(1, BSP_CP0_COUNTFREQ/1000, BSP_CP0_COUNTMARGIN)) {
        OALMSG(OAL_ERROR, (
            L"ERROR: OEMInit: Failed to initialize system clock\r\n"
        ));
        goto cleanUp;
    }        

    // Initialize the KITL connection if required
    KITLIoctl (IOCTL_KITL_STARTUP, NULL, 0, NULL, 0, NULL);

cleanUp:
    OALMSG(OAL_FUNC, (L"-OEMInit\r\n"));
}

VOID OALKitlPowerOn(void)
{
    BOOL PowerState = 1;
    KITLIoctl (IOCTL_KITL_POWER_CALL, &PowerState, sizeof(PowerState), NULL, 0, NULL);
}

VOID OALKitlPowerOff(void)
{
    BOOL PowerState = 0;
    KITLIoctl (IOCTL_KITL_POWER_CALL, &PowerState, sizeof(PowerState), NULL, 0, NULL);
}

void OEMWriteDebugLED (WORD wIndex, DWORD dwPattern)
{
}

//------------------------------------------------------------------------------
