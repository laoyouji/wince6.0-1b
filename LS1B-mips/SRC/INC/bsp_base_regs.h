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
//  File:  bsp_base_regs.h
//
//  This header file defines the addresses of the base registers for
//  the BSP components.
//
//  Note:  File is used for C/C++ and assembler code.
//
//  The following abbreviations are used for different addressing type:
//    PA - physical address
//    UA - uncached virtual address
//    CA - cached virtual address
//    OA - offset address
//
//  The naming convention for BSP base registers is:
//    BSP_REG_<ADDRTYPE>_<SUBSYSTEM>
//
#ifndef __BSP_BASE_REGS_H
#define __BSP_BASE_REGS_H

//------------------------------------------------------------------------------
// PCI Bus windows
//------------------------------------------------------------------------------

#define BSP_REG_PA_PCI_MEM          0x08000000 
#define BSP_REG_PA_PCI_MEMOFFS      0x08000000
#define BSP_REG_PCI_MEMSIZE         0x08000000

#define BSP_REG_PA_PCI_IO           0x14000000
#define BSP_REG_PA_PCI_IOOFFS       0x00000000
#define BSP_REG_PCI_IOSIZE          0x02000000

#define BSP_REG_PA_IOPCI_MEM        0x1D000000
#define BSP_REG_PA_IOPCI_MEMOFFS    0x1D000000
#define BSP_REG_IOPCI_MEMSIZE       0x01000000

#define BSP_REG_PA_IOPCI_IO         0x1C000000
#define BSP_REG_PA_IOPCI_IOOFFS     0x02000000
#define BSP_REG_IOPCI_IOSIZE        0x01000000

//------------------------------------------------------------------------------
// NV RAM, SWITCH, LCD & ALTERA
//------------------------------------------------------------------------------

#define BSP_REG_PA_NVRAM            0x1A000000
#define BSP_REG_PA_SWITCH           0x1A010000
#define BSP_REG_PA_LCD              0x1A020000
#define BSP_REG_PA_ALTERA           0x1A030000

//------------------------------------------------------------------------------
// ALI M1535+ Configuration Port
//------------------------------------------------------------------------------

#define BSP_REG_OA_M1535_CFG        0x03F0
#define BSP_REG_PA_M1535_CFG        (BSP_REG_PA_PCI_IO + 0x03F0)

//------------------------------------------------------------------------------
// ALI M1535+ Interrupt Controller
//------------------------------------------------------------------------------

#define BSP_REG_OA_M1535_PIC1       0x0020
#define BSP_REG_PA_M1535_PIC1       (BSP_REG_PA_PCI_IO + 0x0020)

#define BSP_REG_OA_M1535_PIC2       0x00A0
#define BSP_REG_PA_M1535_PIC2       (BSP_REG_PA_PCI_IO + 0x00A0)

#define BSP_REG_0A_M1535_EDGE1      0x004D0
#define BSP_REG_PA_M1535_EDGE1      (BSP_REG_PA_PCI_IO + 0x04D0)

#define BSP_REG_0A_M1535_EDGE2      0x004D1
#define BSP_REG_PA_M1535_EDGE2      (BSP_REG_PA_PCI_IO + 0x04D1)

//------------------------------------------------------------------------------

#endif

