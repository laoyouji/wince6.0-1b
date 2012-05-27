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
//  File:  flash.c
//
//  This file implements boot loader functions related to image flash.
//  
#include <boot.h>

//------------------------------------------------------------------------------
// Local Variables 

static UINT32 g_flashAddress;
static UINT32 g_flashSize;


//------------------------------------------------------------------------------
//
//  Function:  BLFlashDownload
//
//  This function download image from flash memory to RAM.
//
UINT32 BLFlashDownload()
{
    UINT32 rc = BL_ERROR;
    ROMHDR *pTOC;
    ULONG *pFlash;

    // Check if there is a valid image
    OALLog(L"INFO: Try Boot Image from flash memory\r\n");

    // Make sure that there is image in flash memory
    pFlash = OALPAtoCA(IMAGE_FLASH_PA_START + ROM_SIGNATURE_OFFSET);
    if (*pFlash != ROM_SIGNATURE) {
        OALLog(L"ERROR: Image Signature in Flash Memory not found\r\n");
        goto cleanUp;
    }

    // Get TOC pointer    
    pFlash++;
    pTOC = (ROMHDR*)(
     *pFlash - IMAGE_WINCE_CODE_PA_START + IMAGE_FLASH_PA_START
    );

    // Print out ROMHDR information
    OALLog(L"\r\n");
    OALLog(L"ROMHDR (pTOC = 0x%08x) ---------------------\r\n", pTOC);
    OALLog(L"    DLL First           : 0x%08x\r\n", pTOC->dllfirst);
    OALLog(L"    DLL Last            : 0x%08x\r\n", pTOC->dlllast);
    OALLog(L"    Physical First      : 0x%08x\r\n", pTOC->physfirst);
    OALLog(L"    Physical Last       : 0x%08x\r\n", pTOC->physlast); 
    OALLog(L"    Num Modules         : %10d\r\n",   pTOC->nummods);
    OALLog(L"    RAM Start           : 0x%08x\r\n", pTOC->ulRAMStart); 
    OALLog(L"    RAM Free            : 0x%08x\r\n", pTOC->ulRAMFree); 
    OALLog(L"    RAM End             : 0x%08x\r\n", pTOC->ulRAMEnd); 
    OALLog(L"    Num Copy Entries    : %10d\r\n",   pTOC->ulCopyEntries);
    OALLog(L"    Copy Entries Offset : 0x%08x\r\n", pTOC->ulCopyOffset);  
    OALLog(L"    Prof Symbol Length  : 0x%08x\r\n", pTOC->ulProfileLen);
    OALLog(L"    Prof Symbol Offset  : 0x%08x\r\n", pTOC->ulProfileOffset);
    OALLog(L"    Num Files           : %10d\r\n",   pTOC->numfiles);
    OALLog(L"    Kernel Flags        : 0x%08x\r\n", pTOC->ulKernelFlags);
    OALLog(L"    FileSys RAM Percent : 0x%08x\r\n", pTOC->ulFSRamPercent);
    OALLog(L"    Driver Glob Start   : 0x%08x\r\n", pTOC->ulDrivglobStart);
    OALLog(L"    Driver Glob Length  : 0x%08x\r\n", pTOC->ulDrivglobLen);
    OALLog(L"    CPU                 :     0x%04x\r\n", pTOC->usCPUType);
    OALLog(L"    MiscFlags           :     0x%04x\r\n", pTOC->usMiscFlags);
    OALLog(L"    Extensions          : 0x%08x\r\n", pTOC->pExtensions); 
    OALLog(L"    Tracking Mem Start  : 0x%08x\r\n", pTOC->ulTrackingStart);
    OALLog(L"    Tracking Mem Length : 0x%08x\r\n", pTOC->ulTrackingLen);
    OALLog(L"\r\n");

    // Image base must be same as we expect
    if (pTOC->physfirst != (DWORD)OALPAtoCA(IMAGE_WINCE_CODE_PA_START)) {
        OALLog(L"ERROR: Image in flash memory has incorrect base\r\n");
        goto cleanUp;
    }         

    // Copy image it
    OALLog(L"Copy image from flash to memory...");
    memcpy(
        OALPAtoCA(IMAGE_WINCE_CODE_PA_START), OALPAtoCA(IMAGE_FLASH_PA_START),
        pTOC->physlast - pTOC->physfirst
    );         
    OALLog(L" Done\r\n");

    // Ok, lets jump to image        
    rc = BL_JUMP;
 
cleanUp:
    return rc;
}


//------------------------------------------------------------------------------
//
//  Function:  OEMIsFlashAddr
//
//  This function determines whether the address provided lies in a platform's
//  flash or RAM address range. 
//
BOOL OEMIsFlashAddr(ULONG address)
{
    BOOL rc;

    OALMSG(OAL_FUNC, (L"+OEMIsFlashAddr(0x%08x)\r\n", address));
    
    rc = (
        address >= (UINT32)OALPAtoCA(IMAGE_FLASH_PA_START) && 
        address < (UINT32)OALPAtoCA(IMAGE_FLASH_PA_START + IMAGE_FLASH_SIZE)
    );

    OALMSG(OAL_FUNC, (L"-OEMIsFlashAddr(rc = %d)\r\n", rc));
    return rc;
}


//------------------------------------------------------------------------------
//
//  Function:  OEMStartEraseFlash
//
//  This function is called by the bootloader to initiate the flash memory
//  erasing process.
//
BOOL OEMStartEraseFlash(ULONG address, ULONG size)
{
    BOOL rc = FALSE;

    OALMSG(OAL_FUNC, (
        L"+OEMStartEraseFlash(0x%08x, 0x%08x)\r\n", address, size
    ));
    
    if (
        address < (UINT32)OALPAtoCA(IMAGE_FLASH_PA_START) ||
        size > IMAGE_FLASH_SIZE
    ) {
        OALMSG(OAL_WARN, (
            L"OEMStartEraseFlash: Invalid region (start 0x%08x size 0x%08x)\r\n",
            address, size
        ));
        goto cleanUp;
    }

    // Save address and size for later
    g_flashAddress = address;
    g_flashSize = size;

    rc = TRUE;
    
cleanUp:
    OALMSG(OAL_FUNC, (L"-OEMStartEraseFlash(rc = %d)\r\n", rc));
    return rc;
}



//------------------------------------------------------------------------------
//
//  Function:  OEMContinueEraseFlash
//
//  This function is called by the bootloader to during download to provide
//  ability to continue the flash block erasing operation.
//
void OEMContinueEraseFlash()
{
}


//------------------------------------------------------------------------------
//
//  Function:  OEMFinishEraseFlash
//
//  This function is called by the bootloader to finish flash erase before
//  it will call OEMWriteFlash.
//
BOOL OEMFinishEraseFlash(void)
{
    BOOL rc = FALSE;
    
    OALMSG(OAL_FUNC, (L"+OEMFinishEraseFlash\r\n"));

    OALMSG(OAL_WARN, (
        L"Erase flash memory at 0x%08x size 0x%08x...", 
        g_flashAddress, g_flashSize
    ));

    if (!OALFlashErase(
        OALPAtoUA(IMAGE_FLASH_PA_START), OALPAtoUA((VOID*)g_flashAddress), g_flashSize
    )) {
        OALMSG(OAL_ERROR, (
            L"\r\nERROR: Flash erase for address 0x%08x failed\r\n",
            g_flashAddress
        ));
        goto cleanUp;
    }        

    OALMSG(OAL_WARN, (L" Done\r\n"));
    
    rc = TRUE;

cleanUp:    
    OALMSG(OAL_FUNC, (L"-OEMFinishEraseFlash(rc = %d)\r\n", rc));
    return rc;
}


//------------------------------------------------------------------------------
//
//  Function:  OEMWriteFlash
//
//  This function is called by the bootloader to write the image that may
//  be stored in a RAM file cache area to flash memory.
//
BOOL OEMWriteFlash(ULONG address, ULONG size) 
{
    BOOL rc = FALSE;
    VOID *pSource;

    OALMSG(OAL_FUNC, (L"+OEMWriteFlash(0x%08x, 0x%08x\r\n", address, size));

    // First get location where data are    
    pSource = OEMMapMemAddr(address, address);
    if (pSource == NULL) {
        OALMSG(OAL_ERROR, (
            L"ERROR: OEMMapMemAddr failed for 0x%08x\r\n", address
        ));
        goto cleanUp;
    }        

    OALMSG(OAL_WARN, (
        L"Write flash memory at 0x%08x size 0x%08x...", address, size
    ));

    rc = OALFlashWrite(
        OALPAtoUA(IMAGE_FLASH_PA_START), OALPAtoUA((VOID*)address), size, pSource
    );
    if (!rc) {
        OALMSG(OAL_ERROR, (L"\r\nERROR: Flash write failed\r\n"));
        goto cleanUp;
    }

    OALMSG(OAL_WARN, (L" Done\r\n"));
    
cleanUp:
    OALMSG(OAL_FUNC, (L"-OEMWriteFlash(rc = %d)\r\n", rc));
    return rc;
}

//------------------------------------------------------------------------------
