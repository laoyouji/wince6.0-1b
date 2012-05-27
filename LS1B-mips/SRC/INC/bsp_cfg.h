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
//  File:  bsp_cfg.h
//
#ifndef __BSP_CFG_H
#define __BSP_CFG_H

//------------------------------------------------------------------------------

#define BSP_DEVICE_PREFIX       "SGEAR"         // Device name prefix

//------------------------------------------------------------------------------
// CP0
//------------------------------------------------------------------------------

#define BSP_CP0_STATUS          0x14410000      // Disable parity check
#define BSP_CP0_CONFIG          0x403466F3

#define BSP_CP0_COUNTFREQ       200000000       // 200 MHz
#define BSP_CP0_COUNTMARGIN     256             // Margin for clock operation

//------------------------------------------------------------------------------
// CPU
//------------------------------------------------------------------------------

#define BSP_CPUSTAT             0x0000001C

//------------------------------------------------------------------------------
// Flash/device controller
//------------------------------------------------------------------------------

#define BSP_LDCS0               0x1800004C      // 16MB @ 0x18000000 - 16 bit
#define BSP_LCST0               0x10398E38

#define BSP_LDCS1               0x1900004C      // 16MB @ 0x19000000 - 16 bit
#define BSP_LCST1               0x10398E38

#define BSP_LDCS2               0x1A00000C      // 16MB @ 0x1A000000 - 8 bit
#define BSP_LCST2               0x10398E38

//------------------------------------------------------------------------------
// SDRAM controller
//------------------------------------------------------------------------------

#define BSP_SDRAM01             0x000000A9      // 128MB @ 0x00000000 - 32 bit
#define BSP_SDRAM23             0x00000000      // None
#define BSP_MEMCTRL             0x00001116      // 01: 256Mb*4,8 bit,CAS 3
#define BSP_REFCTRL             0x00000516
#define BSP_REFCTRH             0x00000001

//------------------------------------------------------------------------------
// GPIO controller
//------------------------------------------------------------------------------

#define BSP_GIUFUNSEL           0x00000003      // 1:GIU, 0:shared
#define BSP_GIUDIR              0x00000000      // 1:Output, 0:Input
#define BSP_GIUPIOD             0x00000000      // 1:High, 0:Low
#define BSP_GIUINTEN            0x00000002      // 1:Enable, 0:Disable(Enable GPIO1 for RTC Alarm)
#define BSP_GIUINTTYP           0x00000000      // 1:Edge, 0:Level
#define BSP_GIUINTALSEL         0x00000000      // 1:High, 0:Low
#define BSP_GIUINTHTSEL         0x00000000      // 1:Hold, 0:Discard

//------------------------------------------------------------------------------
// Debug Serial Interface Unit
//------------------------------------------------------------------------------

#define BSP_UART_DSIUDLL        0x03            // 38400Bd
#define BSP_UART_DSIUDLM        0x00            // 38400Bd

//------------------------------------------------------------------------------
// PCI Bus
//------------------------------------------------------------------------------

#define BSP_PCI_W0              0x08000089      // 128MB window @ 0x08000000
#define BSP_PCI_W1              0x1400008B      // 32MB  window @ 0x14000000
#define BSP_PCI_INIT00          0x08000016      // Memory read/write
#define BSP_PCI_INIT10          0x00000012      // I/O read/write

#define BSP_PCI_CTL0L           0x80000000      // 32768 clocks, inf retry
#define BSP_PCI_CTL0H           0x0000001F      // Save address on errors
#define BSP_PCI_ARB0L           0x0000003F      // All masters in group 0
#define BSP_PCI_ARB0H           0x00500011      // Default value

#define BSP_PCI_CMD0            0x0006          // Default value
#define BSP_PCI_MLTIM0          0x80            // PCI latency 0x80
#define BSP_PCI_BARC0           0xFFFFFFFF      // VRC5477 isn't visible
#define BSP_PCI_BARM010         0x00000000      // SDRAM bank 0/1 is visible
#define BSP_PCI_BARM230         0xFFFFFFFF      // SDRAM bank 2/3 isn't visible
#define BSP_PCI_BAR00           0xFFFFFFFF      // LDCS0 isn't visible
#define BSP_PCI_BAR10           0xFFFFFFFF      // LDCS1 isn't visible
#define BSP_PCI_BAR20           0xFFFFFFFF      // LDCS2 isn't visible
#define BSP_PCI_BARB0           0xFFFFFFFF      // BOOT isn't visible
#define BSP_PCI_BARP00          0xFFFFFFFF      // IOPCIW0 isn't visible
#define BSP_PCI_BARP10          0xFFFFFFFF      // IOPCIW1 isn't visible

//------------------------------------------------------------------------------
// IOPCI Bus
//------------------------------------------------------------------------------

#define BSP_IOPCI_W0            0x1D00008C      // 16MB window @ 0x1D000000
#define BSP_IOPCI_W1            0x1C00008C      // 16MB window @ 0x1C000000
#define BSP_PCI_INIT01          0x1D000016      // Memory read/write
#define BSP_PCI_INIT11          0x02000012      // I/O read/write

#define BSP_PCI_CTL1L           0x80000000      // 32768 clocks, inf retry
#define BSP_PCI_CTL1H           0x0000001F      // Save address on errors
#define BSP_PCI_ARB1L           0x0000003F      // All masters in group 0
#define BSP_PCI_ARB1H           0x00500011      // Default value

#define BSP_PCI_CMD1            0x0006          // Default value
#define BSP_PCI_MLTIM1          0x80            // PCI latency 0x80
#define BSP_PCI_BARC1           0xFFFFFFFF      // VRC5477 isn't visible
#define BSP_PCI_BARM011         0x00000000      // SDRAM bank 0/1 is visible
#define BSP_PCI_BARM231         0xFFFFFFFF      // SDRAM bank 2/3 isn't visible
#define BSP_PCI_BAR01           0xFFFFFFFF      // LDCS0 isn't visible
#define BSP_PCI_BAR11           0xFFFFFFFF      // LDCS1 isn't visible
#define BSP_PCI_BAR21           0xFFFFFFFF      // LDCS2 isn't visible
#define BSP_PCI_BARB1           0xFFFFFFFF      // BOOT isn't visible
#define BSP_PCI_BARP01          0xFFFFFFFF      // PCIW0 isn't visible
#define BSP_PCI_BARP11          0xFFFFFFFF      // PCIW1 isn't visible

//------------------------------------------------------------------------------
// PIB
//------------------------------------------------------------------------------

#define BSP_PIBMISC             0x00000007      // USB input buffer enable

//------------------------------------------------------------------------------

#endif
