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
//  File: power.c
//
#include <bsp.h>

//------------------------------------------------------------------------------
//
//  Globals:  g_picx
//
//  This global variables are used to store PIC masks.
//
static UINT8 g_pic1;
static UINT8 g_pic2;

//------------------------------------------------------------------------------
//
//  Function:  BSPPowerOff
//
//  This function is called to prepare system for Power Off mode on board
//  level. For SG2 base board we save PIC interrupt mask and disable all
//  interrupts.
//
VOID BSPPowerOff()
{
    M1535_PIC_REGS *pPIC1Regs, *pPIC2Regs;

    OALMSG(OAL_FUNC&&OAL_POWER, (L"+BSPPowerOff\r\n"));

    pPIC1Regs = (M1535_PIC_REGS*)OALPAtoUA(BSP_REG_PA_M1535_PIC1);
    pPIC2Regs = (M1535_PIC_REGS*)OALPAtoUA(BSP_REG_PA_M1535_PIC2);
     
    g_pic1 = INPORT8(&pPIC1Regs->mask);
    g_pic2 = INPORT8(&pPIC2Regs->mask);

    OUTPORT8(&pPIC1Regs->mask, 0xFF);
    OUTPORT8(&pPIC2Regs->mask, 0xFF);

    OALMSG(OAL_FUNC&&OAL_POWER, (L"-BSPPowerOff\r\n"));
}    

//------------------------------------------------------------------------------
//
//  Function:  BSPPowerOn
//
//  This function is called to restore system after Power Off mode on board
//  level. For SG2 base board we restore PIC interrupt masks.
//
VOID BSPPowerOn()
{
    M1535_PIC_REGS *pPIC1Regs, *pPIC2Regs;

    OALMSG(OAL_FUNC&&OAL_POWER, (L"+BSPPowerOn\r\n"));

    pPIC1Regs = (M1535_PIC_REGS*)OALPAtoUA(BSP_REG_PA_M1535_PIC1);
    pPIC2Regs = (M1535_PIC_REGS*)OALPAtoUA(BSP_REG_PA_M1535_PIC2);

    OUTPORT8(&pPIC1Regs->mask, g_pic1);
    OUTPORT8(&pPIC2Regs->mask, g_pic2);

    OALMSG(OAL_FUNC&&OAL_POWER, (L"-BSPPowerOn\r\n"));
}       

//------------------------------------------------------------------------------

