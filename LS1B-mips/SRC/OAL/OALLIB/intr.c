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
//  File:  intr.c
//
//  This file implements interrupt BSP callback functions --- it must be
//  implemented because on SG2 motherboard is secondary interrupt controller
//  in M1535+ chip
//
#include <bsp.h>

//------------------------------------------------------------------------------
//
//  Definitions:  IRQ_PIC_x
//
//
#define IRQ_PIC_0           32
#define IRQ_PIC_1           33
#define IRQ_PIC_2           34
#define IRQ_PIC_3           35
#define IRQ_PIC_4           36
#define IRQ_PIC_5           37
#define IRQ_PIC_6           38
#define IRQ_PIC_7           39

#define IRQ_PIC_8           40
#define IRQ_PIC_9           41
#define IRQ_PIC_10          42
#define IRQ_PIC_11          43
#define IRQ_PIC_12          44
#define IRQ_PIC_13          45
#define IRQ_PIC_14          46
#define IRQ_PIC_15          47

//------------------------------------------------------------------------------

static UINT32 g_irqTable[4][4] = {
   { IRQ_INTA,   IRQ_INTB,   IRQ_PIC_11, IRQ_INTD   },
   { IRQ_INTB,   IRQ_PIC_11, IRQ_INTD,   IRQ_INTA   },
   { IRQ_PIC_11, IRQ_INTD,   IRQ_INTA,   IRQ_INTB   },
   { IRQ_INTD,   IRQ_INTA,   IRQ_INTB,   IRQ_PIC_11 }
};   


//------------------------------------------------------------------------------

BOOL OALRTCAlarmIntrHandler(); // define in rtc.c

//------------------------------------------------------------------------------

static VRC5477_REGS *g_pVRC5477Regs;
static M1535_PIC_REGS *g_pPIC1Regs;
static M1535_PIC_REGS *g_pPIC2Regs;


//------------------------------------------------------------------------------
//
//  Function:  BSPIntrInit
//
//  This function is called from OALIntrInit to initialize secondary interrupt
//  controller.
//
BOOL BSPIntrInit()
{
    UINT8 *pPIC1Edge, *pPIC2Edge;
    UINT32 irq;
    
    OALMSG(OAL_INTR&&OAL_FUNC, (L"+BSPIntrInit\r\n"));

    // Add static mapping for on chip devices...
    OALIntrStaticTranslate(SYSINTR_FIRMWARE + 0, IRQ_UART1);   // UART1

    // Add GPIO static mapping for RTC alarm
    OALIntrStaticTranslate(SYSINTR_RTC_ALARM, IRQ_GPIO);
    // And enable it (it will not occur until it is set in OEMSetAlarmTime)
    irq = IRQ_GPIO; OALIntrEnableIrqs(1, &irq);


    // Get and save uncached virtual addresses for VRC5477 and PIC1/PIC2
    g_pVRC5477Regs = OALPAtoUA(VRC5477_REG_PA);
    g_pPIC1Regs = OALPAtoUA(BSP_REG_PA_M1535_PIC1);
    g_pPIC2Regs = OALPAtoUA(BSP_REG_PA_M1535_PIC2);

    // M1535+ INTR uses positive logic (active on high), set VRC5477
    CLRPORT32(&g_pVRC5477Regs->INTPPES0, 1 << 4);

    // We have to enable PCI interrupts
    irq = IRQ_INTA; OALIntrEnableIrqs(1, &irq);
    irq = IRQ_INTB; OALIntrEnableIrqs(1, &irq);
    irq = IRQ_INTC; OALIntrEnableIrqs(1, &irq);
    irq = IRQ_INTD; OALIntrEnableIrqs(1, &irq);

    // PIC1/PIC2 edge registers are needed only for initialization
    pPIC1Edge = OALPAtoUA(BSP_REG_PA_M1535_EDGE1);
    pPIC2Edge = OALPAtoUA(BSP_REG_PA_M1535_EDGE2);
    
    // Initialize the 8259 PIC1
    OUTREG8(&g_pPIC1Regs->ctrl, 0x11);          // ICW1, cascade & ICW4
    OUTREG8(&g_pPIC1Regs->mask, IRQ_PIC_0);     // ICW2, vector to 32
    OUTREG8(&g_pPIC1Regs->mask, 0x04);          // ICW3, slave on IRQ 2
    OUTREG8(&g_pPIC1Regs->mask, 0x01);          // ICW4, normal EOI
    OUTREG8(&g_pPIC1Regs->ctrl, 0x0B);          // OCW2, read IS register
    OUTREG8(&g_pPIC1Regs->mask, 0xFF);          // OCW1, all disabled
    
    // Now initialize the 8259 PIC2
    OUTREG8(&g_pPIC2Regs->ctrl, 0x11);          // ICW1, cascade & ICW4
    OUTREG8(&g_pPIC2Regs->mask, IRQ_PIC_8);     // ICW2, vector to 40
    OUTREG8(&g_pPIC2Regs->mask, 0x02);          // ICW3, we are IRQ 2
    OUTREG8(&g_pPIC2Regs->mask, 0x01);          // ICW4, normal EOI
    OUTREG8(&g_pPIC2Regs->ctrl, 0x0B);          // OCW2, read IS register
    OUTREG8(&g_pPIC2Regs->mask, 0xFF);          // OCW1, all disabled

    // IRQ0-IRQ7 Edge sensitive(IRQ2 cannot be set to level sensitive)
    OUTREG8(pPIC1Edge, 0x00);

    // IRQ9&IRQ11 level (USB host, PCI INTC), other edge sensitive
    OUTREG8(pPIC2Edge, 0x0A);

    // Enable interrupt from PIC2 on PIC1
    CLRREG8(&g_pPIC1Regs->mask, 1 << 2);

    // Set static interrupt mappings for legacy devices
    
    OALIntrStaticTranslate(SYSINTR_FIRMWARE + 8, IRQ_PIC_1);    // keyboard
    OALIntrStaticTranslate(SYSINTR_FIRMWARE + 9, IRQ_PIC_12);   // mouse

    // We are done    
    OALMSG(OAL_INTR&&OAL_FUNC, (L"-BSPIntrInit(rc = 1)\r\n"));
    return TRUE;
}


//------------------------------------------------------------------------------
//
//  Function:  BSPIntrRequestIrqs
//
//  This function is called from OALIntrRequestIrq to get IRQs for device on
//  position defined in first parameter. It allows to map IRQs for BSP specific
//  devices.
//
BOOL BSPIntrRequestIrqs(DEVICE_LOCATION *pDevLoc, UINT32 *pCount, UINT32 *pIrqs)
{
    BOOL rc = FALSE;
    OAL_PCI_LOCATION pciLoc;
    UINT8 bus, pin, dev0, dev1, shift, header;

    OALMSG(OAL_INTR&&OAL_FUNC, (
        L"+BSPIntrRequestIrqs(%d/%d/0x%08x/%d, 0x%08x, 0x%08x)\r\n",
        pDevLoc->IfcType, pDevLoc->BusNumber, pDevLoc->LogicalLoc,
        pDevLoc->Pin, pCount, pIrqs
    ));

    // This shouldn't happen
    if (*pCount < 1) goto cleanUp;

    // Only PCI bus devices...
    if (pDevLoc->IfcType != PCIBus) goto cleanUp;

    // First we need to find interrupt shift 
    bus = ((OAL_PCI_LOCATION*)&pDevLoc->LogicalLoc)->bus;
    shift = 0;
    dev0 = dev1 = 0;
    pciLoc.bus = 0;
    
    while (pciLoc.bus < bus) {
        for (pciLoc.dev = 0; pciLoc.dev < 32; pciLoc.dev++) {
            // Expect only bridge on function 0
            pciLoc.fnc = 0;
            // Get device header
            header = OALPCIGetHeaderType(pDevLoc->BusNumber, pciLoc);
            if ((header & ~PCI_MULTIFUNCTION) != PCI_BRIDGE_TYPE) continue;
            // If target bus isn't behinde this bus skip
            if (bus > OALPCIGetSubBusNum(pDevLoc->BusNumber, pciLoc)) continue;
            // With this bridge position interrupt shift (ignore board bridge)
            if (pciLoc.bus > 1) shift = (shift + pciLoc.dev) & 0x03;
            // Save first two bridge device position
            if (pciLoc.bus == 0) dev0 = pciLoc.dev;
            if (pciLoc.bus == 1) dev1 = pciLoc.dev;
            // Now look behinde bridge
            pciLoc.bus = OALPCIGetSecBusNum(pDevLoc->BusNumber, pciLoc);
            // No more devices to look for on this bus
            break;
        }
    }

    // Now we know shift for potencial bridges...
    pciLoc = *(OAL_PCI_LOCATION*)&pDevLoc->LogicalLoc;
    pin = (UINT8)(pDevLoc->Pin - 1);

    // First we must solve behind bridge situation
    if (pciLoc.bus > 1) {
       if (dev0 == 9) {
          // Bridge is connected to slot 1
          pin = (UINT8)((pin + shift + pciLoc.dev) & 0x03);
          pciLoc.bus = 0;
          pciLoc.dev = 9;
       } else if (dev0 == 10) {
          // Bridge is connected to slot 2
          pin = (UINT8)((pin + shift + pciLoc.dev) & 0x03);
          pciLoc.bus = 0;
          pciLoc.dev = 10;
       } else if (dev0 == 6 && dev1 == 5) {
          // Bridge is connected to slot 3
          pin = (UCHAR)((pin + shift + pciLoc.dev) & 0x03);
          pciLoc.bus = 1;
          pciLoc.dev = 5;
       } else if (dev0 == 6 && dev1 == 6) {
          // Bridge is connected to slot 4
          pin = (UCHAR)((pin + shift + pciLoc.dev) & 0x03);
          pciLoc.bus = 1;
          pciLoc.dev = 6;
       } else {
            goto cleanUp;
       }
    }     

    // On board devices & PCI slots
    if (pciLoc.bus == 0) {
        switch (pciLoc.dev) {
        case 5: // M1535+ IDE
            pIrqs[0] = IRQ_PIC_14;
            *pCount = 1;                
            rc = TRUE;
            break;
        case 7: // M1535+ Audio 97
            pIrqs[0] = IRQ_PIC_6;
            *pCount = 1;                
            rc = TRUE;
            break;
        case 9: // PCI Slot 1
            pIrqs[0] = g_irqTable[1][pin];
            *pCount = 1;                
            rc = TRUE;
            break;
        case 10: // PCI Slot 2
            pIrqs[0] = g_irqTable[0][pin];
            *pCount = 1;                
            rc = TRUE;
            break;
        case 11: // M1535+ USBD
            pIrqs[0] = IRQ_PIC_9;
            *pCount = 1;                
            rc = TRUE;
            break;
        }
    } else if (pciLoc.bus == 1) {
        switch (pciLoc.dev) {
        case 1: // Am79c973
            pIrqs[0] = IRQ_INTB;
            *pCount = 1;                
            rc = TRUE;
            break;
        case 5: // PCI Slot 3
            pIrqs[0] = g_irqTable[3][pin];
            *pCount = 1;                
            rc = TRUE;
             break;
        case 6: // PCI Slot 4
            pIrqs[0] = g_irqTable[0][pin];
            *pCount = 1;                
            rc = TRUE;
            break;
        }
    }

cleanUp:    
    if (!rc) *pCount = 0;
    OALMSG(OAL_INTR&&OAL_FUNC, (L"-BSPIntrRequestIrqs(rc = %d)\r\n", rc));
    return rc;
}


//------------------------------------------------------------------------------
//
//  Function:  BSPIntrEnableIrq
//
//  This function is called from OALIntrEnableIrq to enable interrupt on
//  secondary interrupt controller.
//
UINT32 BSPIntrEnableIrq(UINT32 irq)
{
    OALMSG(OAL_INTR&&OAL_VERBOSE, (L"+BSPIntrEnableIrq(%d)\r\n", irq));
    
    // Secondary IRQ are using IRQ_PIC_0 and above
    if (irq < IRQ_PIC_0) goto cleanUp;

    // Enable interrupt on appropriate PIC
    if (irq < IRQ_PIC_8) {
        CLRREG8(&g_pPIC1Regs->mask, 1 << (irq - IRQ_PIC_0));
    } else {
        CLRREG8(&g_pPIC2Regs->mask, 1 << (irq - IRQ_PIC_8));
        CLRREG8(&g_pPIC1Regs->mask, 1 << 2);
    }            

    // It is using IRQ_PCIINTC for cascade
    irq = IRQ_INTC;
    
cleanUp:
    OALMSG(OAL_INTR&&OAL_VERBOSE, (L"-BSPIntrEnableIrq(irq = %d)\r\n", irq));
    return irq;
}


//------------------------------------------------------------------------------
//
//  Function:  BSPIntrDisableIrq
//
//  This function is called from OALIntrDisableIrq to disable interrupt on
//  secondary interrupt controller.
//
UINT32 BSPIntrDisableIrq(UINT32 irq)
{
    OALMSG(OAL_INTR&&OAL_VERBOSE, (L"+BSPIntrDisableIrq(%d)\r\n", irq));

    // Secondary IRQ are using IRQ_GPIO16 and above
    if (irq < IRQ_PIC_0) goto cleanUp;

    // Disable interrupt on appropriate PIC
    if (irq < IRQ_PIC_8) {
        SETREG8(&g_pPIC1Regs->mask, 1 << (irq - IRQ_PIC_0));
    } else {
        SETREG8(&g_pPIC2Regs->mask, 1 << (irq - IRQ_PIC_8));
    }            

    // We are done....
    irq = OAL_INTR_IRQ_UNDEFINED;
    
cleanUp:
    OALMSG(OAL_INTR&&OAL_VERBOSE, (L"-BSPIntrDisableIrq(irq = %d\r\n", irq));
    return irq;
}


//------------------------------------------------------------------------------
//
//  Function:  BSPIntrDoneIrq
//
//  This function is called from OALIntrDoneIrq to finish interrupt on
//  secondary interrupt controller.
//
UINT32 BSPIntrDoneIrq(UINT32 irq)
{
    OALMSG(OAL_INTR&&OAL_VERBOSE, (L"+BSPIntrDoneIrq(%d)\r\n", irq));

    // Secondary IRQ are using IRQ_GPIO16 and above
    if (irq < IRQ_PIC_0) goto cleanUp;

    // Enable interrupt on appropriate PIC
    if (irq < IRQ_PIC_8) {
        CLRREG8(&g_pPIC1Regs->mask, 1 << (irq - IRQ_PIC_0));
    } else {
        CLRREG8(&g_pPIC2Regs->mask, 1 << (irq - IRQ_PIC_8));
    }
    
    // IRQ_GPIO5 is used for cascade
    irq = IRQ_INTC;
    
cleanUp:
    OALMSG(OAL_INTR&&OAL_VERBOSE, (L"-BSPIntrDoneIrq(irq = %d)\r\n", irq));
    return irq;
}


//------------------------------------------------------------------------------
//
//  Function:  BSPIntrActiveIrq
//
//  This function is called from interrupt handler to give BSP chance to 
//  translate IRQ in case of secondary interrupt controller.
//
UINT32 BSPIntrActiveIrq(UINT32 irq)
{
    UINT8 data;
    UINT32 doneIrq;

    OALMSG(OAL_INTR&&OAL_VERBOSE, (L"+BSPIntrActiveIrq(%d)\r\n", irq));

    switch(irq)
    {
    case IRQ_GPIO:
        // Check whether alarm happen
        if(OALRTCAlarmIntrHandler() == FALSE) {
            irq = OAL_INTR_IRQ_UNDEFINED;
        }
        // Acknowledge GPIO interrupt
        OUTREG32(&g_pVRC5477Regs->GIUINSTAT, 0xffffffff);
        // Re-enable IRQ_GPIO interrupt on VRC5477
        doneIrq = IRQ_GPIO;
        OALIntrDoneIrqs(1, &doneIrq);
        break;
    case IRQ_INTC:
        // Read PIC1 interrupt
        OUTREG8(&g_pPIC1Regs->ctrl, 0x0E);
        data = INREG8(&g_pPIC1Regs->data) & 0x07;
        if (data != 2) {
            irq = IRQ_PIC_0 + data;
            SETREG8(&g_pPIC1Regs->mask, 1 << data);
        } else {
            // Read PIC2 interrupt
            OUTREG8(&g_pPIC2Regs->ctrl, 0x0E);
            data = INREG8(&g_pPIC2Regs->data) & 0x07;
            irq = IRQ_PIC_8 + data;
            SETREG8(&g_pPIC2Regs->mask, 1 << data);
            // End interrupt on PIC2
            OUTREG8(&g_pPIC2Regs->ctrl, 0x20);
        }
        // End interrupt on PIC1
        OUTREG8(&g_pPIC1Regs->ctrl, 0x20);

        // Re-enable IRQ_INTC interrupt on VRC5477
        doneIrq = IRQ_INTC;
        OALIntrDoneIrqs(1, &doneIrq);
        break;
    }

    OALMSG(OAL_INTR&&OAL_VERBOSE, (L"-BSPIntrActiveIrq(%d)\r\n", irq));
    return irq;
}

//------------------------------------------------------------------------------

