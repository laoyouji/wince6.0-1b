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
//  File:  eth.c
//
#include <boot.h>


//------------------------------------------------------------------------------
//
//  Static: g_ethState
//
//  This structure contains local state variables.
//
static struct {
    EDBG_ADDR edbgAddress;
    OAL_KITL_ETH_DRIVER *pDriver;
    UINT8 buffer[OAL_KITL_BUFFER_SIZE];
} g_ethState;


//------------------------------------------------------------------------------
//
//  Function:  BLEthDownload
//
//  This function initialize ethernet adapter and call download function from
//  bootloader common library.
//
UINT32 BLEthDownload(BOOT_CFG *pConfig, OAL_KITL_DEVICE *pBootDevices)
{
    UINT32 rc = BL_ERROR;
    OAL_KITL_DEVICE *pDevice;
    UINT32 subnetMask, *pLeaseTime, leaseTime = 0;
    UINT16 mac[3];
    BOOL jumpToImage = FALSE;

    // First we need configure device bus
    switch (pConfig->bootDevLoc.IfcType) {
    case PCIBus:
        if (!OALPCIConfig(
            0, BSP_REG_PA_PCI_MEMOFFS, BSP_REG_PCI_MEMSIZE, 
            BSP_REG_PA_PCI_IOOFFS + 0x1000, BSP_REG_PCI_IOSIZE, 1, 
            (OAL_PCI_LOCATION*)&pConfig->bootDevLoc.LogicalLoc
        )) {
           OALLog(L"ERROR: PCI Bus configuration failed\r\n");
           goto cleanUp;
        }
    }

    // Find boot/KITL device driver
    pDevice = OALKitlFindDevice(&pConfig->bootDevLoc, pBootDevices);        
    if (pDevice == NULL || pDevice->type != OAL_KITL_TYPE_ETH) {
        OALLog(L"ERROR: Boot device doesn't exist or it is unsupported\r\n");
        goto cleanUp;
    }

    // Get device driver
    g_ethState.pDriver = (OAL_KITL_ETH_DRIVER*)pDevice->pDriver;
    
    // Call InitDmaBuffer if there is any
    if (g_ethState.pDriver->pfnInitDmaBuffer != NULL) {
        if (!g_ethState.pDriver->pfnInitDmaBuffer(
            (UINT32)g_ethState.buffer, sizeof(g_ethState.buffer)
        )) {
            OALLog(L"ERROR: Boot device driver InitDmaBuffer call failed\r\n");
            goto cleanUp;
        }
    }      

    // Call Init
    if (!g_ethState.pDriver->pfnInit(
        pConfig->bootDevLoc.PhysicalLoc, pConfig->bootDevLoc.LogicalLoc, mac
    )) {
       OALLog(L"ERROR: Boot device driver Init call failed\r\n");
       goto cleanUp;
    }         

    OALLog(L"INFO: Boot device uses MAC %s\r\n", OALKitlMACtoString(mac));

    // Generate device name if there isn't any
    if (pConfig->deviceId[0] == '\0') {
        OALKitlCreateName(BSP_DEVICE_PREFIX, mac, pConfig->deviceId);
    }      

    OALLog(L"INFO: *** Device Name %S ***\r\n", pConfig->deviceId);

    // Initialize ethernet
    memset(&g_ethState.edbgAddress, 0, sizeof(g_ethState.edbgAddress));

    // Set lease time pointer to activate DHCP or update global structure
    if ((pConfig->kitlFlags & OAL_KITL_FLAGS_DHCP) != 0) {
       pLeaseTime = &leaseTime;
    } else {
       pLeaseTime = NULL;
       g_ethState.edbgAddress.dwIP = pConfig->ipAddress;
    }
    memcpy(
        g_ethState.edbgAddress.wMAC, mac, sizeof(g_ethState.edbgAddress.wMAC)
    );
    subnetMask = 0xFFFFFF;
       
    // Initialize TFTP transport
    // Note that first parameter must be pointer to global variable...
    if (!EbootInitEtherTransport(
       &g_ethState.edbgAddress, &subnetMask, &jumpToImage, pLeaseTime,
       BOOT_VERSION_MAJOR, BOOT_VERSION_MINOR, BSP_DEVICE_PREFIX, 
       pConfig->deviceId, EDBG_CPU_TYPE_MIPS|0x0F, 0
    )) {
       OALLog(L"ERROR: EbootInitEtherTransport call failed\r\n");
       goto cleanUp;
    }

    // If DHCP was used update lease time and address
    if (pLeaseTime != NULL) pConfig->ipAddress = g_ethState.edbgAddress.dwIP;

    // Depending on what we get from TFTP
    rc = jumpToImage ? BL_JUMP : BL_DOWNLOAD;

cleanUp:
    return rc;
}


//------------------------------------------------------------------------------
//
//  Function:  BLEthConfig
//
//  This function receives connect message from host and it updates arguments
//  based on it. Most part of connect message isn't used by KITL anymore, but
//  we want set passive KITL mode when no-KITL is select on PB.
//
VOID BLEthConfig(BSP_ARGS *pArgs)
{
    EDBG_OS_CONFIG_DATA *pConfig;

    // Get host connect info
    pConfig = EbootWaitForHostConnect(&g_ethState.edbgAddress, NULL);

    // Update BSP arguments based on connect info
    pArgs->kitl.flags &= ~OAL_KITL_FLAGS_PASSIVE;
    if ((pConfig->KitlTransport & KTS_PASSIVE_MODE) != 0) {
        pArgs->kitl.flags |= OAL_KITL_FLAGS_PASSIVE;
    }        
}


//------------------------------------------------------------------------------
//
//  Function:  OEMEthGetFrame
//
//  Check to see if a frame has been received, and if so copy to buffer.
//  An optimization  which may be performed in the Ethernet driver is to 
//  filter out all received broadcast packets except for ARPs.
//
BOOL OEMEthGetFrame(BYTE *pData, UINT16 *pLength)
{
    return g_ethState.pDriver->pfnGetFrame(pData, pLength) > 0;
}


//------------------------------------------------------------------------------
//
//  Function:  OEMEthSendFrame
//
//  Send Ethernet frame.  
//
BOOL OEMEthSendFrame(BYTE *pData, DWORD length)
{
    return g_ethState.pDriver->pfnSendFrame(pData, length) == 0;
}

