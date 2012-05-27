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
//  File:  debug.c
//
#include <boot.h>


//------------------------------------------------------------------------------
//
//  Function:  OEMInitDebugSerial
//
//  Initialize debug serial port
//
BOOL OEMDebugInit()
{
    VRC5477_REGS *pVRC5477Regs = OALPAtoUA(VRC5477_REG_PA);

    // Set baud rate
    OUTREG8(&pVRC5477Regs->UARTLCR0, UART_LCR_DLAB);
    OUTREG8(&pVRC5477Regs->UARTDLL0, BSP_UART_DSIUDLL);
    OUTREG8(&pVRC5477Regs->UARTDLM0, BSP_UART_DSIUDLM);

    // 8 bit, 1 stop bit, no parity
    OUTREG8(&pVRC5477Regs->UARTLCR0, UART_LCR_8BIT);

    // Enable FIFO
    OUTREG8(&pVRC5477Regs->UARTFCR0, UART_FCR_EN);

    // Pool
    OUTREG8(&pVRC5477Regs->UARTIER0, 0);

    // Set DTR/RTS signals
    OUTREG8(&pVRC5477Regs->UARTMCR0, UART_MCR_DTR|UART_MCR_RTS);

    return TRUE;
}


//------------------------------------------------------------------------------
//
//  Function:  OEMWriteDebugByte
//
//  Write byte to debug serial port.
//
VOID OEMWriteDebugByte(UINT8 ch)
{
    VRC5477_REGS *pVRC5477Regs = OALPAtoUA(VRC5477_REG_PA);

    // wait FIFO empty
    while (!(INREG8(&pVRC5477Regs->UARTLSR0) & UART_LSR_THRE));
    // send
    OUTREG8(&pVRC5477Regs->UARTTHR0, ch);
}


//------------------------------------------------------------------------------
//
//  Function:  OEMWriteDebugString
//
//  Output unicode string to debug serial port
//
VOID OEMWriteDebugString(UINT16 *string)
{
    while (*string != L'\0') OEMWriteDebugByte((UINT8)*string++);
}


//------------------------------------------------------------------------------
//
//  Function:  OEMReadDebugByte
//
//  Input character/byte from debug serial port
//
INT OEMReadDebugByte()
{
     VRC5477_REGS *pVRC5477Regs = OALPAtoUA(VRC5477_REG_PA);
     UINT8 ch, status;
    
     status = INREG8(&pVRC5477Regs->UARTLSR0);
     if ((status & UART_LSR_DR) != 0) {
       ch = INREG8(&pVRC5477Regs->UARTRBR0);
       if ((status & UART_LSR_RFERR) != 0) ch = OEM_DEBUG_COM_ERROR;
    } else {
       ch = OEM_DEBUG_READ_NODATA;
    }
    return ch;
}


//------------------------------------------------------------------------------
//
//  Function:  OEMClearDebugCommError
//
//  Clear debug serial port error
//
VOID OEMClearDebugCommError()
{
}

//------------------------------------------------------------------------------
