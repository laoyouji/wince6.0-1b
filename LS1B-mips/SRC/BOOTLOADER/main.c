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
//  File:  main.c
//
#include <boot.h>
#include <kitl_cfg.h>
#include <boot_cfg.h>

//------------------------------------------------------------------------------
// External Functions

void JumpTo(VOID *pAddress);

//------------------------------------------------------------------------------
//
//  Global: g_bootCfg
//
//  This global variable used to save boot configuration. It is readed from
//  flash memory or initialized to default values if flash memory doesn't
//  contain valid structure. It can be modified by user in bootloader
//  configuration menu invoked by BLMenu.
//
BOOT_CFG g_bootCfg;

//------------------------------------------------------------------------------
//
//  Global: g_bootPage
//
//  This variable says from which boot loader page boot code was invoked.
//  This value must be known for flash config paramters in correct place.
//  See boot.bib for more info about boot loader page.
//
UINT32 g_bootPage;


//------------------------------------------------------------------------------
//
//  Function:  OEMPlatformInit
//
//  This function provide platform initialization functions. It is called
//  from boot loader after OEMDebugInit is called.  Note that boot loader
//  BootloaderMain is called from startup.s code which is run after reset.
//
BOOL OEMPlatformInit(void)
{
    VRC5477_REGS *pVRC5477Regs = OALPAtoUA(VRC5477_REG_PA);
    UINT8 *pAlteraRegs = OALPAtoUA(BSP_REG_PA_ALTERA);
    UINT8 *pSwitchRegs = OALPAtoUA(BSP_REG_PA_SWITCH);
    UINT8 dipSwitch, cpuModel, sysClock, pcbRevision;

    OALLog(
        L"Microsoft Windows CE Loader %d.%d for NEC SG2_Vr5500 Built %S at %S\r\n",
        BOOT_VERSION_MAJOR, BOOT_VERSION_MINOR, __DATE__, __TIME__
    );

    // Initialize global cache constants
    OALCacheGlobalsInit();
   
    //-----------------------------------------------------------------------
    // Initialize North Bridge VRC5477
    //-----------------------------------------------------------------------
    
    // Set CPU Status Register (TMODE - no parity error)
    OUTREG32(&pVRC5477Regs->CPUSTAT, BSP_CPUSTAT);

    // Setup local bus registers - flash 0
    OUTREG32(&pVRC5477Regs->LDCS0, BSP_LDCS0);
    OUTREG32(&pVRC5477Regs->LCST0, BSP_LCST0);

    // Setup local bus registers - flash 1
    OUTREG32(&pVRC5477Regs->LDCS1, BSP_LDCS1);
    OUTREG32(&pVRC5477Regs->LCST1, BSP_LCST1);

    // Setup local bus registers - board devices
    OUTREG32(&pVRC5477Regs->LDCS2, BSP_LDCS2);
    OUTREG32(&pVRC5477Regs->LCST2, BSP_LCST2);

    // Initialize GPIO
    OUTREG32(&pVRC5477Regs->GIUFUNSEL, BSP_GIUFUNSEL);
    OUTREG32(&pVRC5477Regs->GIUPIOD, BSP_GIUPIOD);
    OUTREG32(&pVRC5477Regs->GIUDIR, BSP_GIUDIR);
    OUTREG32(&pVRC5477Regs->GIUINTTYP, BSP_GIUINTTYP);
    OUTREG32(&pVRC5477Regs->GIUINTALSEL, BSP_GIUINTALSEL);
    OUTREG32(&pVRC5477Regs->GIUINTHTSEL, BSP_GIUINTHTSEL);
    OUTREG32(&pVRC5477Regs->GIUINSTAT, 0xffffffff); // Clear noise interrupt request
    OUTREG32(&pVRC5477Regs->GIUINTEN, BSP_GIUINTEN); // should be last line to avoid unexpected interrupt

    // Setup PIB registers
    OUTREG32(&pVRC5477Regs->PIBMISC, BSP_PIBMISC);

    // Initialize PCI bus
    OALPCIInit();

    //-----------------------------------------------------------------------
    // Read mother board info
    //-----------------------------------------------------------------------

    // Read Altera-chip registers
    OUTREG8(pAlteraRegs, 1);
    cpuModel = INREG8(pAlteraRegs);
    OUTREG8(pAlteraRegs, 2);
    sysClock = INREG8(pAlteraRegs);
    OUTREG8(pAlteraRegs, 3);
    pcbRevision = INREG8(pAlteraRegs);
   
    // Read DIP switch
    dipSwitch = INREG8(pSwitchRegs);
   
    OALLog(
        L"CPU Model %d  SysClock %d  PCB Revision %d  DIP2 0x%02x\r\n",
        cpuModel, sysClock, pcbRevision, dipSwitch
    );
   
    // Print message about flash setting      
    if ((dipSwitch & 0x04) == 0) {
        OALLog(L"Flash Disabled (DIP2-3 on), ");
    } else {
        OALLog(L"Flash Enabled (DIP2-3 off), ");
    }
   
    // Print page about flash boot page
    if ((dipSwitch & 0x10) != 0) {
        g_bootPage = 0;
        OALLog(L"Flat Boot (DIP2-5 off)\r\n");
    } else {
        g_bootPage = dipSwitch >> 5;
        OALLog(L"Paged Boot on page %d (DIP2-5 on)\r\n", g_bootPage);
    }

    return TRUE;
}

//------------------------------------------------------------------------------
//
//  Function:  OEMPreDownload
//
//  This function is called before downloading an image. There is place
//  where user can be asked about device setup.
//
ULONG OEMPreDownload()
{
    ULONG rc = BL_ERROR;
    BOOT_CFG *pCfg = OALPAtoCA(IMAGE_BOOT_CONFIG_PA_START);

    OALLog(L"INFO: Predownload....\r\n");
    
    // First try to check if there is an eboot config structure
    if (
        pCfg->signature == BOOT_CFG_SIGNATURE && 
        pCfg->version == BOOT_CFG_VERSION
    ) {
        OALLog(L"INFO: Boot configuration found at 0x%08x\r\n", pCfg);
        memcpy(&g_bootCfg, pCfg, sizeof(g_bootCfg));
    } else {
        OALLog(
            L"WARN: Boot config wasn't found at 0x%08x, using defaults\r\n", 
            pCfg
        );
        memset(&g_bootCfg, 0, sizeof(g_bootCfg));
        g_bootCfg.signature = BOOT_CFG_SIGNATURE;
        g_bootCfg.version = BOOT_CFG_VERSION;
        g_bootCfg.bootDevLoc.IfcType = PCIBus;
        g_bootCfg.bootDevLoc.BusNumber = 0;
        g_bootCfg.bootDevLoc.LogicalLoc = 0x00010100;
        g_bootCfg.bootDevLoc.Pin = 1;
        g_bootCfg.kitlFlags = OAL_KITL_FLAGS_ENABLED|OAL_KITL_FLAGS_DHCP|OAL_KITL_FLAGS_VMINI;
        g_bootCfg.kitlDevLoc.IfcType = PCIBus;
        g_bootCfg.kitlDevLoc.BusNumber = 0;
        g_bootCfg.kitlDevLoc.LogicalLoc = 0x00010100;
        g_bootCfg.kitlDevLoc.Pin = 1;
        g_bootCfg.ipAddress = 0;
        g_bootCfg.ipMask = 0;
        g_bootCfg.ipRoute = 0;
    }

    // We must initialize PCI bus first
    OALPCIConfig(0, 0, 0, 0, 0, 0, NULL);

    // Call configuration menu
    BLMenu();

    // Image download depend on protocol
    switch (OALKitlDeviceType(&g_bootCfg.bootDevLoc, g_bootDevices)) {
    case OAL_KITL_TYPE_FLASH:
        rc = BLFlashDownload();
        break;
    case OAL_KITL_TYPE_ETH:
        rc = BLEthDownload(&g_bootCfg, g_bootDevices);
        break;
    }

    return rc;
}


//------------------------------------------------------------------------------
//
//  Function:  OEMReadData
//
//  This function is called to read data from the transport during
//  the download process.
//
BOOL OEMReadData(ULONG size, UCHAR *pData)
{
    return EbootEtherReadData(size, pData);
}

//------------------------------------------------------------------------------
//
// Function:     OEMShowProgress
//
// Description:  This function is called during the download process 
//               to visualise download progress
//

void OEMShowProgress(ULONG packetNumber)
{
}


//------------------------------------------------------------------------------
//
//  Function:  OEMLaunch
//
//  This function is the last one called by the boot framework and it is 
//  responsible for to launching the image.
//
void OEMLaunch(
   ULONG start, ULONG size, ULONG launch, const ROMHDR *pRomHeader
) {

    BSP_ARGS *pArgs = OALPAtoCA(IMAGE_SHARE_ARGS_PA_START);

    OALMSG(OAL_FUNC, (
        L"+OEMLaunch(0x%08x, 0x%08x, 0x%08x, 0x%08x)\r\n", start, size,
        launch, pRomHeader
    ));

    // Initialize ARGS structure
    memset(pArgs, 0, IMAGE_SHARE_ARGS_SIZE);
    pArgs->header.signature = OAL_ARGS_SIGNATURE;
    pArgs->header.oalVersion = OAL_ARGS_VERSION;
    pArgs->header.bspVersion = BSP_ARGS_VERSION;

    memcpy(pArgs->deviceId, g_bootCfg.deviceId, sizeof(pArgs->deviceId));
    pArgs->kitl.flags = g_bootCfg.kitlFlags;
    pArgs->kitl.devLoc = g_bootCfg.kitlDevLoc;

    // Following code set KITL device parameters
    switch (OALKitlDeviceType(&g_bootCfg.kitlDevLoc, g_kitlDevices)) {
    case OAL_KITL_TYPE_ETH:
        pArgs->kitl.ipAddress = g_bootCfg.ipAddress;
        pArgs->kitl.ipMask = g_bootCfg.ipMask;
        pArgs->kitl.ipRoute = g_bootCfg.ipRoute;
        break;
    }

    // Depending on protocol there can be some action required
    switch (OALKitlDeviceType(&g_bootCfg.bootDevLoc, g_bootDevices)) {
    case OAL_KITL_TYPE_ETH:
        BLEthConfig(pArgs);
        break;
    }

    // If launch address is in flash memory area
    if (OEMIsFlashAddr(launch)) {

        // Check if we download bootloader, if so halt
        if (
            start >= (UINT32)OALPAtoCA(IMAGE_FLASH_PA_START) &&
            size < IMAGE_BOOT_FLASH_SIZE
        ) {
            OALLog(
                L"INFO: Launch address in boot address space, spin forever...\r\n"
            );
            while (TRUE);
        }

        // Otherwise we should copy image to memory
        if (BLFlashDownload() != BL_JUMP) {
            OALLog(L"ERROR: Failed copy image from flash, spin forever...\r\n");
            while (TRUE);
        }
        launch = (ULONG)OALPAtoCA(IMAGE_WINCE_CODE_PA_START);

    } else if (launch == 0) {        

        // If launch address is NULL change it to default address
        launch = (ULONG)OALPAtoCA(IMAGE_WINCE_CODE_PA_START);

    }            

    // Print message, flush caches and jump to image
    OALLog(L"INFO: Lauch Windows CE by jumping to 0x%08x...\r\n", launch);
    OALFlushDCache();
    OALFlushICache();
    JumpTo(OALCAtoUA(launch));
   
    // We never should get there
    OALLog(L"ERROR: Kernel returns back from launch, spin forever...\r\n");
    while (TRUE);
}


//------------------------------------------------------------------------------
//
//  Function:  OEMMapMemAddr
//
//  This function maps image relative address to memory address. It is used
//  by boot loader to verify some parts of downloaded image.
//
UINT8* OEMMapMemAddr(DWORD image, DWORD address)
{
    UINT8 *pAddress;

    OALMSG(OAL_FUNC, (L"+OEMMapMemAddr(0x%08x, 0x%08x)\r\n", image, address));

    // Map address if it is in flash memory area
    if (OEMIsFlashAddr(address)) {
        pAddress = (UINT8*)(
            address - image + (UINT32)OALPAtoCA(IMAGE_BOOT_FRAM_PA_START)
        );
    } else {
        pAddress = (UINT8*)address;
    }

    OALMSG(OAL_FUNC, (L"-OEMMapMemAddr(pAddress = 0x%08x)\r\n", pAddress));
    return pAddress;
}


//------------------------------------------------------------------------------
//
//  Function:  OALGetTickCount
//
//  This function returns relative time in milliseconds. To avoid fast rollover
//  (for 396 MHz it is 10 seconds) code trace last counter value and offset.
//         
UINT32 OALGetTickCount()
{
    static UINT32 base = 0, offset = 0;
    UINT32 plus, delta;

    delta = OALTimerGetCount() - base;
    plus = delta/(BSP_CP0_COUNTFREQ/1000);
    offset += plus;
    base += plus * (BSP_CP0_COUNTFREQ/1000);
    return offset;
}


//------------------------------------------------------------------------------
//
//  Function:  OEMKitlGetSecs
//
//  This function returns relative time in seconds.
//
DWORD OEMKitlGetSecs()
{
    return OALGetTickCount()/1000;
}


//------------------------------------------------------------------------------
//
//  Function: OALStall
//
VOID OALStall(UINT32 delay)
{
    UINT32 base, counts;

    counts = delay * (BSP_CP0_COUNTFREQ/1000000);
    base = OALTimerGetCount();
    while ((OALTimerGetCount() - base) < counts);
}

//
// The following three functions are used in KITL device initialization
// (platform\common\src\common\kitl\device.c). 
//


//------------------------------------------------------------------------------
//
//  Function: OEMIoControl - only handle IOCTL_HAL_DDK_CALL for PCI information
//
BOOL OEMIoControl(
    DWORD code, VOID *pInBuffer, DWORD inSize, VOID *pOutBuffer, DWORD outSize,
    DWORD *pOutSize
) {
    BOOL rc = FALSE;
    if (IOCTL_HAL_DDK_CALL == code) {
        rc = OALIoCtlHalDdkCall (code, pInBuffer, inSize, pOutBuffer, outSize, pOutSize);
    }
    return rc;
}

//------------------------------------------------------------------------------
//
//  Function: NKPhysToVirt - convert physical address to virtual address
//
LPVOID NKPhysToVirt (DWORD dwShiftedPhysAddr, BOOL fCached)
{
    return (LPVOID) ((dwShiftedPhysAddr << 8)
        | (fCached? 0x80000000 : 0xa0000000));
}

//------------------------------------------------------------------------------
//
//  Function: NKSetLastError - stub function 
//
VOID NKSetLastError (DWORD dwErr)
{
}

//------------------------------------------------------------------------------


