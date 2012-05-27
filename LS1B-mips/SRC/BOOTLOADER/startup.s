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
//  Definition
//
//  Following macros are used to convert from physical to kernel virtual
//  address space
//
#define KSEG0_BASE      0x80000000
#define KSEG1_BASE      0xA0000000

        .text

//------------------------------------------------------------------------------
//
//  Function:  StartUp
//
//  This code is run after hardware reset, runtime reset and when system wakes
//  from sleep. The current hardware state depends on event type but - most
//  hardware must be initialized there. The function does necessary hardware
//  initialization and detect if it is wakeup. In such case it gets saved
//  address and jump on it. Otherwise boot loader is copied to memory and
//  it is runned.
//
LEAF_ENTRY(StartUp)
        .set    noreorder
        .set    noat

        //---------------------------------------------------------------
        // Disable watch exception
        //---------------------------------------------------------------

        mtc0    zero, watchlo
        mtc0    zero, watchhi

        //---------------------------------------------------------------
        // Setup status & config CP0 registers
        //---------------------------------------------------------------

        li      t0, BSP_CP0_STATUS 
        mtc0    t0, psr
        
        li      t0, BSP_CP0_CONFIG
        mtc0    t0, config

        //---------------------------------------------------------------
        // SDRAM configuration
        //---------------------------------------------------------------

        li      t0, VRC5477_REG_PA|KSEG1_BASE
        li      t1, BSP_SDRAM01
        sw      t1, VRC5477_REG_OA_SDRAM01(t0)
        li      t1, BSP_SDRAM23
        sw      t1, VRC5477_REG_OA_SDRAM23(t0)
        li      t1, BSP_MEMCTRL
        sw      t1, VRC5477_REG_OA_MEMCTRL(t0)
        li      t1, BSP_REFCTRL
        sw      t1, VRC5477_REG_OA_REFCTRL(t0)
        li      t1, BSP_REFCTRH
        sw      t1, VRC5477_REG_OA_REFCTRH(t0)

        //---------------------------------------------------------------
        // Copy loader to RAM
        //---------------------------------------------------------------
        // Copy the bootloader code to RAM (allows for faster execution
        // and we can update flash while the loader is running)
        li      t1, IMAGE_BOOT_FLASH_SIZE
        li      t2, IMAGE_BOOT_FLASH_PA_START|KSEG1_BASE
        li      t3, IMAGE_BOOT_CODE_PA_START|KSEG1_BASE
        addu    t0, t2, t1
20:     lw      t4, 0(t2)
        lw      t5, 4(t2)
        lw      t6, 8(t2)
        lw      t7, 12(t2)
        addu    t2, t2, 16
        sw      t4, 0(t3)
        sw      t5, 4(t3)
        sw      t6, 8(t3)
        sw      t7, 12(t3)
        bne     t0, t2, 20b
        addu    t3, t3, 16

        //---------------------------------------------------------------
        // Initialize caches
        //---------------------------------------------------------------
        // Note: Assembler/linker assumes that this code is running
        // from RAM/KSEG0 but in reality it run in ROM/KSEG1. This is
        // reason why OALCacheInit address must be moved to KSEG1
        // (and code copying must be before this). On return code
        // will run from KSEG0.
        
        la      t0, OALCacheInit
        li      t1, KSEG1_BASE
        or      t0, t1
        la      ra, 30f
        jr      t0
        nop
30:

        //---------------------------------------------------------------
        // Jump to boot loader main code
        //---------------------------------------------------------------

        li      sp, IMAGE_BOOT_STACK_PA_START|KSEG0_BASE + IMAGE_BOOT_STACK_SIZE
        la      t0, BootloaderMain
        jr      t0
        nop

        .set    reorder
        .set    at
        .end    StartUp

//------------------------------------------------------------------------------
//
// Function:     JumpTo
//
// Description:  Simple function which allows jump to address which is passed
//               as argument.
//

LEAF_ENTRY(JumpTo)

        .set    noreorder

#ifdef MIPSIV
        // Make sure the 64-bit value in a0 is correctly sign-extended.
        dsll32 a0, a0, 0
        dsra32 a0, a0, 0
#endif

        j       a0
        nop

        .end    Launch

//------------------------------------------------------------------------------

