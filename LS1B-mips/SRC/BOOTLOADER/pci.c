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
//  File:  pci.c
//
#include <boot.h>

//------------------------------------------------------------------------------
//
//  Function:  OALPCIInit
//
BOOL OALPCIInit()
{
    VRC5477_REGS *pVRC5477Regs = OALPAtoUA(VRC5477_REG_PA);
    M1535_CFG_REGS *pM1535Regs = OALPAtoUA(BSP_REG_PA_M1535_CFG);
    OAL_PCI_LOCATION pciLoc;
    UINT32 u32;

    //----------------------------------------------------------------------
    // External PCI
    //----------------------------------------------------------------------
    
    // Cold reset
    OUTREG32(&pVRC5477Regs->PCICTL0H, PCI_CTRL_CRST);
    OALStall(10000);
    OUTREG32(&pVRC5477Regs->PCICTL0H, 0);
    OALStall(10000);
    
    // Setup windows
    OUTREG32(&pVRC5477Regs->PCIINIT00, BSP_PCI_INIT00);
    OUTREG32(&pVRC5477Regs->PCIW0, BSP_PCI_W0);
    OUTREG32(&pVRC5477Regs->PCIINIT10, BSP_PCI_INIT10);
    OUTREG32(&pVRC5477Regs->PCIW1, BSP_PCI_W1);

    // Setup control & arbiter registers
    OUTREG32(&pVRC5477Regs->PCICTL0L, BSP_PCI_CTL0L);
    OUTREG32(&pVRC5477Regs->PCICTL0H, BSP_PCI_CTL0H);
    OUTREG32(&pVRC5477Regs->PCIARB0L, BSP_PCI_ARB0L);
    OUTREG32(&pVRC5477Regs->PCIARB0H, BSP_PCI_ARB0H);
        
    // Setup configuration space
    OUTREG16(&pVRC5477Regs->PCICMD0, BSP_PCI_CMD0);
    OUTREG8(&pVRC5477Regs->MLTIM0, BSP_PCI_MLTIM0);
    OUTREG32(&pVRC5477Regs->BARC0, BSP_PCI_BARC0);
    OUTREG32(&pVRC5477Regs->BARM010, BSP_PCI_BARM010);
    OUTREG32(&pVRC5477Regs->BARM230, BSP_PCI_BARM230);
    OUTREG32(&pVRC5477Regs->BAR00, BSP_PCI_BAR00);
    OUTREG32(&pVRC5477Regs->BAR10, BSP_PCI_BAR10);
    OUTREG32(&pVRC5477Regs->BAR20, BSP_PCI_BAR20);
    OUTREG32(&pVRC5477Regs->BARB0, BSP_PCI_BARB0);
    OUTREG32(&pVRC5477Regs->BARP00, BSP_PCI_BARP00);
    OUTREG32(&pVRC5477Regs->BARP10, BSP_PCI_BARP10);

    //----------------------------------------------------------------------
    // Internal PCI
    //----------------------------------------------------------------------

    OUTREG32(&pVRC5477Regs->PCICTL1H, PCI_CTRL_CRST);
    OALStall(10000);
    OUTREG32(&pVRC5477Regs->PCICTL1H, PCI_CTRL_CRST);
    OALStall(10000);

    // Setup internal PCI windows
    OUTREG32(&pVRC5477Regs->PCIINIT01, BSP_PCI_INIT01);
    OUTREG32(&pVRC5477Regs->IOPCIW0, BSP_IOPCI_W0);
    OUTREG32(&pVRC5477Regs->PCIINIT11, BSP_PCI_INIT11);
    OUTREG32(&pVRC5477Regs->IOPCIW1, BSP_IOPCI_W1);

    // Setup control & arbiter registers
    OUTREG32(&pVRC5477Regs->PCICTL1L, BSP_PCI_CTL1L);
    OUTREG32(&pVRC5477Regs->PCICTL1H, BSP_PCI_CTL1H);
    OUTREG32(&pVRC5477Regs->PCIARB1L, BSP_PCI_ARB1L);
    OUTREG32(&pVRC5477Regs->PCIARB1H, BSP_PCI_ARB1H);
    
    // Setup configuration space
    OUTREG16(&pVRC5477Regs->PCICMD1, BSP_PCI_CMD1);
    OUTREG8(&pVRC5477Regs->MLTIM1, BSP_PCI_MLTIM1);
    OUTREG32(&pVRC5477Regs->BARC1, BSP_PCI_BARC1);
    OUTREG32(&pVRC5477Regs->BARM011, BSP_PCI_BARM011);
    OUTREG32(&pVRC5477Regs->BARM231, BSP_PCI_BARM231);
    OUTREG32(&pVRC5477Regs->BAR01, BSP_PCI_BAR01);
    OUTREG32(&pVRC5477Regs->BAR11, BSP_PCI_BAR11);
    OUTREG32(&pVRC5477Regs->BAR21, BSP_PCI_BAR21);
    OUTREG32(&pVRC5477Regs->BARB1, BSP_PCI_BARB1);
    OUTREG32(&pVRC5477Regs->BARP01, BSP_PCI_BARP01);
    OUTREG32(&pVRC5477Regs->BARP11, BSP_PCI_BARP11);

    OALStall(10000);

    //----------------------------------------------------------------------
    // ALI M1535+ South Bridge
    //----------------------------------------------------------------------
    // Is there ALI M1535+ bridge = CPU board is inserted to SG2 mother board,
    // in such case we must do some initialization --- default config address
    // lines for some ALI M1535+ internal devices colide with PCI slot config
    // address lines.
    pciLoc.bus = 0;
    pciLoc.dev = 8;
    pciLoc.fnc = 0;
    OALPCICfgRead(0, pciLoc, 0, sizeof(u32), &u32);
    if (u32 != 0x153310B9) goto cleanUp;

    //----------------------------------------------------------------------
    // PCI-ISA bridge initialize
    //----------------------------------------------------------------------

    OALLog(L"INFO: OALPCIInit: ALI M1535+ Bridge detected\r\n");

    u32 = 0x0000C119;   // I/O control, select PS2 keyboad/mouse
    OALPCICfgWrite(0, pciLoc, 0x40, sizeof(u32), &u32);
    u32 = 0x0000025D;   // Primary IDE IRQ14
    OALPCICfgWrite(0, pciLoc, 0x44, sizeof(u32), &u32);
    u32 = 0x70000009;   // Audio->IRQ6, PCI INTC->IRQ11
    OALPCICfgWrite(0, pciLoc, 0x48, sizeof(u32), &u32);
    u32 = 0x00000000;   // USB1 enable
    OALPCICfgWrite(0, pciLoc, 0x50, sizeof(u32), &u32);
    u32 = 0x00000000;   // PCSJ
    OALPCICfgWrite(0, pciLoc, 0x54, sizeof(u32), &u32);
    u32 = 0x0000007C;   // IDE IDSEL(AD15), INTR
    OALPCICfgWrite(0, pciLoc, 0x58, sizeof(u32), &u32);
    u32 = 0x00004000;   // Document recommend???
    OALPCICfgWrite(0, pciLoc, 0x6C, sizeof(u32), &u32);
    u32 = 0x002600D2;   // PMU IDSEL(AD14), USB IDSEL(AD13)
    OALPCICfgWrite(0, pciLoc, 0x70, sizeof(u32), &u32);
    u32 = 0x40801F01;   // No modem, USB INTR(IRQ09), 2nd IDE IRQ15, AC97 IDSEL(AD17)
    OALPCICfgWrite(0, pciLoc, 0x74, sizeof(u32), &u32);
    u32 = 0x00000000;   // USB2 disable
    OALPCICfgWrite(0, pciLoc, 0x7C, sizeof(u32), &u32);

    //---------------------------
    // Configure super I/O chip
    //---------------------------
    
    OUTPORT8(&pM1535Regs->config, 0x51);    // Enter config mode
    OUTPORT8(&pM1535Regs->config, 0x23);    
    
    // Enable parallel port
    OUTPORT8(&pM1535Regs->index, 0x07);
    OUTPORT8(&pM1535Regs->data,  0x03);     // Select logical device 3
    OUTPORT8(&pM1535Regs->index, 0x30);
    OUTPORT8(&pM1535Regs->data,  0x01);     // Enable device
    OUTPORT8(&pM1535Regs->index, 0x60);
    OUTPORT8(&pM1535Regs->data,  0x03);     // I/O address: 378h
    OUTPORT8(&pM1535Regs->index, 0x61);
    OUTPORT8(&pM1535Regs->data,  0x78);     // I/O address: 378h
    OUTPORT8(&pM1535Regs->index, 0x70);
    OUTPORT8(&pM1535Regs->data,  0x07);     // Irq: 7
    
    // Enable UART1
    OUTPORT8(&pM1535Regs->index, 0x07);
    OUTPORT8(&pM1535Regs->data,  0x04);     // Select logical device 4
    OUTPORT8(&pM1535Regs->index, 0x30);
    OUTPORT8(&pM1535Regs->data,  0x01);     // Enable device
    OUTPORT8(&pM1535Regs->index, 0x60);
    OUTPORT8(&pM1535Regs->data,  0x03);     // I/O address: 3F8h
    OUTPORT8(&pM1535Regs->index, 0x61);
    OUTPORT8(&pM1535Regs->data,  0xF8);     // I/O address: 3F8h
    OUTPORT8(&pM1535Regs->index, 0x70);
    OUTPORT8(&pM1535Regs->data,  0x04);     // Irq: 4
    
    // Enable UART2/INFRA
    OUTPORT8(&pM1535Regs->index, 0x07);
    OUTPORT8(&pM1535Regs->data,  0x05);     // Select logical device 5
    OUTPORT8(&pM1535Regs->index, 0x30);
    OUTPORT8(&pM1535Regs->data,  0x01);     // Enable device
    OUTPORT8(&pM1535Regs->index, 0x60);
    OUTPORT8(&pM1535Regs->data,  0x03);     // I/O address: 3E8h
    OUTPORT8(&pM1535Regs->index, 0x61);
    OUTPORT8(&pM1535Regs->data,  0xE8);     // I/O address: 3E8h
    OUTPORT8(&pM1535Regs->index, 0x70);
    OUTPORT8(&pM1535Regs->data,  0x05);     // Irq: 5
    
    // Enable PS/2 controller
    OUTPORT8(&pM1535Regs->index, 0x07);
    OUTPORT8(&pM1535Regs->data,  0x07);     // Select logical device 7.
    OUTPORT8(&pM1535Regs->index, 0x30);
    OUTPORT8(&pM1535Regs->data,  0x01);     // Enable device
    OUTPORT8(&pM1535Regs->index, 0x70);
    OUTPORT8(&pM1535Regs->data,  0x01);     // Irq: 1 - keyboard
    OUTPORT8(&pM1535Regs->index, 0x72);
    OUTPORT8(&pM1535Regs->data,  0x0C);     // Irq: 12 - mouse
    
    // Enable UART3
    OUTPORT8(&pM1535Regs->index, 0x07);
    OUTPORT8(&pM1535Regs->data,  0x0B);     // Select logical device 11
    OUTPORT8(&pM1535Regs->index, 0x30);
    OUTPORT8(&pM1535Regs->data,  0x01);     // Enable device
    OUTPORT8(&pM1535Regs->index, 0x60);
    OUTPORT8(&pM1535Regs->data,  0x02);     // I/O address: 2F8h
    OUTPORT8(&pM1535Regs->index, 0x61);
    OUTPORT8(&pM1535Regs->data,  0xF8);     // I/O address: 2F8h
    OUTPORT8(&pM1535Regs->index, 0x70);
    OUTPORT8(&pM1535Regs->data,  0x03);     // Irq: 3
    
    // Exit config mode
    OUTPORT8(&pM1535Regs->config, 0xBB);
    
cleanUp:    
    return TRUE;
}

//------------------------------------------------------------------------------
//
//  Function:  OALPCITransBusAddress
//
BOOL OALPCITransBusAddress(
    UINT32 busId, UINT64 busAddress, UINT32 *pAddressSpace, 
    UINT64 *pSystemAddress
) {
    BOOL rc = FALSE;

    OALMSG(OAL_PCI&&OAL_FUNC, (
        L"+OALPCITranslateBusAddress(%d, 0x%08x%08x, %d)\r\n",
        busId, (UINT32)(busAddress >> 32), (UINT32)busAddress, *pAddressSpace
    ));

    if (pAddressSpace == NULL || pSystemAddress == NULL) goto cleanUp;

    if (busId == 0) {
        switch (*pAddressSpace) {
        case 0:
            if (
                busAddress >= BSP_REG_PA_PCI_MEMOFFS &&
                busAddress < BSP_REG_PA_PCI_MEMOFFS + BSP_REG_PCI_MEMSIZE
            ) {
                *pSystemAddress = (
                    busAddress - BSP_REG_PA_PCI_MEMOFFS + BSP_REG_PA_PCI_MEM
                );
                rc = TRUE;
            }
            break;
        case 1:
            if (
                busAddress >= BSP_REG_PA_PCI_IOOFFS &&
                busAddress < BSP_REG_PA_PCI_IOOFFS + BSP_REG_PCI_IOSIZE
            ) {
                *pSystemAddress = (
                    busAddress - BSP_REG_PA_PCI_IOOFFS + BSP_REG_PA_PCI_IO
                );                        
                *pAddressSpace = 0;
                rc = TRUE;
            }
            break;
        }
    } else if (busId == 1) {
        switch (*pAddressSpace) {
        case 0:
            if (
                busAddress >= BSP_REG_PA_IOPCI_MEMOFFS &&
                busAddress < BSP_REG_PA_IOPCI_MEMOFFS + BSP_REG_IOPCI_MEMSIZE
            ) {
                *pSystemAddress = (
                    busAddress - BSP_REG_PA_IOPCI_MEMOFFS + BSP_REG_PA_IOPCI_MEM
                );
                rc = TRUE;
            }
            break;
        case 1:
            if (
                busAddress >= BSP_REG_PA_IOPCI_IOOFFS &&
                busAddress < BSP_REG_PA_IOPCI_IOOFFS + BSP_REG_IOPCI_IOSIZE
            ) {
                *pSystemAddress = (
                    busAddress - BSP_REG_PA_IOPCI_IOOFFS + BSP_REG_PA_IOPCI_IO
                );                        
                *pAddressSpace = 0;
                rc = TRUE;
            }
            break;
        }
    }

cleanUp:
    OALMSG(OAL_PCI&&OAL_FUNC, (
        L"-OALPCITransBusAddress(addressSpace = %d, "
        L"systemAddress = 0x%08x%08x, rc = %d)\r\n", *pAddressSpace, 
        (UINT32)(*pSystemAddress >> 32), (UINT32)*pSystemAddress, rc
    ));      
    return rc;    
}

//------------------------------------------------------------------------------

BOOL INTERRUPTS_ENABLE(BOOL enable)
{
    return FALSE;
}

//------------------------------------------------------------------------------

