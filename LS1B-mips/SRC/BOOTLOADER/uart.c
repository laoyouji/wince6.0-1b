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
//  File: uart.c
//
#include <bsp.h>


//------------------------------------------------------------------------------
//
//  Function:   VRC5477Uart1Init
//
//  Initialize debug serial port
//
BOOL VRC5477Uart1Init(KITL_SERIAL_INFO *pInfo)
{
    BOOL rc = FALSE;
    VRC5477_REGS *pVRC5477Regs = OALPAtoUA(VRC5477_REG_PA);
    UINT16 divider;
    UINT8 lcr;

    // Check if config paramters are supportable
    if (
        pInfo->baudRate < 9600 || pInfo->baudRate > 115200 ||
        pInfo->dataBits != 8 || pInfo->stopBits == 0 || pInfo->parity > 2
    ) goto cleanUp;

    // Input frequency is 1.8462MHz
    divider = (1846200/pInfo->baudRate + 7)/16;

    // Get LCR
    lcr = UART_LCR_8BIT;
    if (pInfo->stopBits > 1) lcr |= UART_LCR_2STP;
    if (pInfo->parity > 0) {
        lcr |= UART_LCR_PE;
        if (pInfo->parity > 1) lcr |= UART_LCR_EP;
    }
    
    // Disable interrupts
    OUTREG8(&pVRC5477Regs->UARTIER1, 0);

    // Set baud rate
    OUTREG8(&pVRC5477Regs->UARTLCR1, UART_LCR_DLAB);
    OUTREG8(&pVRC5477Regs->UARTDLL1, (UINT8)divider);
    OUTREG8(&pVRC5477Regs->UARTDLM1, (UINT8)(divider >> 8));

    // 8 bit, 1 stop bit, no parity
    OUTREG8(&pVRC5477Regs->UARTLCR1, lcr);

    // Enable&reset FIFOs
    OUTREG8(&pVRC5477Regs->UARTFCR1, UART_FCR_EN|UART_FCR_RRST|UART_FCR_TRST);

    // Set DTR&RTS signals
    OUTREG8(&pVRC5477Regs->UARTMCR1, UART_MCR_DTR|UART_MCR_RTS);

    // Clear comm errors
    INREG8(&pVRC5477Regs->UARTLSR1);

    // Best size is full KITL_MTU...
    pInfo->bestSize = KITL_MTU;

    // Done
    rc = TRUE;
    
cleanUp:
    return rc;
}

//------------------------------------------------------------------------------
//
//  Function:  VRC5477Uart1Send
//
UINT16 VRC5477Uart1Send(UINT8 *pData, UINT16 size)
{
    VRC5477_REGS *pVRC5477Regs = OALPAtoUA(VRC5477_REG_PA);

    // This should not happen, but to be sure
    if (size == 0) return 0;
    // Wait FIFO empty
    while (!(INREG8(&pVRC5477Regs->UARTLSR1) & UART_LSR_THRE));
    // Send
    OUTREG8(&pVRC5477Regs->UARTTHR1, *pData);
    // We send only one char per call
    return 1;
}

//------------------------------------------------------------------------------
//
//  Function:  VRC5477Uart1Recv
//
UINT16 VRC5477Uart1Recv(UINT8 *pData, UINT16 size)
{
    VRC5477_REGS *pVRC5477Regs = OALPAtoUA(VRC5477_REG_PA);
    UINT8 status;
    UINT16 count;

    count = 0;
    while (count < size) {
        // Get line status
        status = INREG8(&pVRC5477Regs->UARTLSR1);
        // If there ara no data, break loop
        if ((status & UART_LSR_DR) == 0) break;
        // On error clear FIFOs and break loop
        if ((status & 0x0E) != 0) {
            OUTREG8(&pVRC5477Regs->UARTFCR1, 0x07);
            count = 0;
            break;
        }
        // Get data
        *pData++ = INREG8(&pVRC5477Regs->UARTRBR1);
        count++;
    }        
    return count;
}

//------------------------------------------------------------------------------
//
//  Function:  VRC5477Uart1EnableInts
//
VOID VRC5477Uart1EnableInts()
{
}

//------------------------------------------------------------------------------
//
//  Function:  VRC5477Uart1DisableInts
//
VOID VRC5477Uart1DisableInts()
{
}

//------------------------------------------------------------------------------

