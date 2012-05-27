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

#include <kxmips.h>

#define __MIPS_ASSEMBLER
#include <vrc5477_base_regs.h>
#include <vrc5477_all.h>
#include <bsp_cfg.h>
#include <image_cfg.h>

//------------------------------------------------------------------------------
//
//  Definitions: KSEGx_BASE
//
//  MIPS KSEG0 and KSEG1 base addresses
//
#define KSEG0_BASE      0x80000000
#define KSEG1_BASE      0xA0000000

        .text

//------------------------------------------------------------------------------
//
//  Function:  StartUp
//
//  This code is entry point to Windows CE OS. On device with loader it is 
//  responsibility of loader to do basic hardware initialization. 
//
LEAF_ENTRY(StartUp)
        .set    noreorder
        .set    noat

        // Disable cache parity check, set FR bit
        li      t0, BSP_CP0_STATUS
        mtc0    t0, psr
        nop
        nop

        // Jump to KernelStart
        la      v0, KernelStart
        j       v0
        nop

        .set    reorder
        .set    at
        .end    StartUp

//------------------------------------------------------------------------------
