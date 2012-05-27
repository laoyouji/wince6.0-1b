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
//  File:  rtc.c
//
#include <windows.h>
#include <oal.h>
#include <nkintr.h>
#include "vr0300_rtc.h"

//------------------------------------------------------------------------------
// define critical sections to prevent re-entrance of RTC functions
//
CRITICAL_SECTION RTC_critsect;

//------------------------------------------------------------------------------
// Save latest alarm time
//
static SYSTEMTIME stAlarmTime;

//------------------------------------------------------------------------------
// Utility routines for Accessing DS1386 RTC on RockHopper baseboard
//
BOOL DS_SetAlarmTime(LPSYSTEMTIME lpst);
BOOL DS_GetRealTime(LPSYSTEMTIME lpst);
BOOL DS_SetRealTime(LPSYSTEMTIME lpst);

//------------------------------------------------------------------------------
// Utility routines for converting RTC register values into decimal values
//
UINT16  RegValToDecimal(UINT8);
UINT8   DecimalToRegVal(UINT16);

//------------------------------------------------------------------------------
//
//  Function:  OEMSetAlarmTime
//
//  This function is called by the kernel to set the real-time clock alarm.
//
BOOL OEMSetAlarmTime(LPSYSTEMTIME lpst) 
{
    BOOL rc;

    EnterCriticalSection(&RTC_critsect);
    rc=DS_SetAlarmTime(lpst);
    LeaveCriticalSection(&RTC_critsect);

    return rc;
}

//------------------------------------------------------------------------------
//
//  Function:  OEMGetRealTime
//
//  This function is called by the kernel to retrieve the time from
//  the real-time clock.
//
BOOL OEMGetRealTime(LPSYSTEMTIME lpst) 
{
    EnterCriticalSection(&RTC_critsect);
    DS_GetRealTime(lpst);
    LeaveCriticalSection(&RTC_critsect);

    return TRUE;
}

//------------------------------------------------------------------------------
//
//  Function:  OEMSetRealTime
//
//  This function is called by the kernel to set the real-time clock.
//
BOOL OEMSetRealTime(LPSYSTEMTIME lpst) 
{
	BOOL rc;

    EnterCriticalSection(&RTC_critsect);
    rc=DS_SetRealTime(lpst);
    LeaveCriticalSection(&RTC_critsect);

    return rc;
}

//------------------------------------------------------------------------------
//
//  Function:  OALRTCInit
//
//  This function initialize DS1386.
//
void OALRTCInit()
{
    DS1386_RTC_REGS *pRTCRegs = OALPAtoUA(BSP_REG_PA_NVRAM);

    OALMSG(OAL_RTC&&OAL_FUNC, (L"+OALRTCInit()\r\n"));

    // Disable WatchDog Alarm
    OUTREG8(&pRTCRegs->CMD_REG, (INREG8(&pRTCRegs->CMD_REG) | RTC_CMDREG_WAM));
    OUTREG8(&pRTCRegs->AL_WATCHDOG[0], 0);
    OUTREG8(&pRTCRegs->AL_WATCHDOG[1], 0);

    // Disable and setup Time-Of-Day Alarm
    OUTREG8(&pRTCRegs->CMD_REG, (INREG8(&pRTCRegs->CMD_REG) | RTC_CMDREG_TDM));  // Disable
    OUTREG8(&pRTCRegs->CMD_REG, (INREG8(&pRTCRegs->CMD_REG) | RTC_CMDREG_IPSW)); // Select INTA
    OUTREG8(&pRTCRegs->CMD_REG, (INREG8(&pRTCRegs->CMD_REG) & ~RTC_CMDREG_PU));  // Level trigger
    g_pOemGlobal->dwAlarmResolution = 60*1000; // 60s due to DS1386 hardware limitation

    // Enable RTC oscillator
    OUTREG8(&pRTCRegs->MONTH, (INREG8(&pRTCRegs->MONTH) & ~RTC_EOSC));

    // Set TE bit to enable transfer between external and internal clock registers
    OUTREG8(&pRTCRegs->CMD_REG, (INREG8(&pRTCRegs->CMD_REG) | RTC_CMDREG_TE));

    OALMSG(OAL_RTC&&OAL_FUNC, (L"-OALRTCInit()\r\n"));
}

//------------------------------------------------------------------------------
//
//  Function:  OALIoCtlHalInitRTC
//
//  This function is called by WinCE OS to initialize the time after boot. 
//  Input buffer contains SYSTEMTIME structure with default time value. If
//  hardware has persistent real time clock it will ignore this value
//  (or all call).
//
BOOL OALIoCtlHalInitRTC(
    UINT32 code, VOID *pInpBuffer, UINT32 inpSize, VOID *pOutBuffer, 
    UINT32 outSize, UINT32 *pOutSize
) {
    DS1386_RTC_REGS *pRTCRegs = OALPAtoUA(BSP_REG_PA_NVRAM);
    SYSTEMTIME *lpst   = NULL;
    BOOL       retVal  = FALSE;

    OALMSG(OAL_RTC&&OAL_FUNC, (L"+OALIoCtlHalInitRTC()\r\n"));

    if(pOutSize)
    {
        *pOutSize = 0; 
    }

    // Validity checks
    if((code       != IOCTL_HAL_INIT_RTC) ||
       (pInpBuffer == NULL)               ||
       (inpSize    != sizeof(SYSTEMTIME)))
    {
        OALMSG(OAL_ERROR, (L"ERROR: Invalid calling parameters...returning\r\n"));
        retVal = FALSE;
        goto initExit;
    }
       
    // Call OEMSetRealTime
    lpst    = (SYSTEMTIME *)pInpBuffer;
    retVal  = OEMSetRealTime(lpst); 

initExit:
    OALMSG(OAL_RTC&&OAL_FUNC, (L"-OALIoCtlHalInitRTC(rc = %d)\r\n", retVal));

    return retVal;
}


//------------------------------------------------------------------------------
//
//  Function: OALRTCAlarmIntrHandler
//
//  This function implement rtc alarm interrupt handler. It is called from common
//  MIPS interrupt handler.
//
BOOL OALRTCAlarmIntrHandler()
{
    BOOL isAlarm = FALSE;
    SYSTEMTIME stCurTime;
    DS1386_RTC_REGS *pRTCRegs = OALPAtoUA(BSP_REG_PA_NVRAM);

    // DS1386 can just set alarm's day of week, hour and minute, so
    // we have to compare alarm's year, month and day with current time
    DS_GetRealTime(&stCurTime);
    if(stCurTime.wYear ==stAlarmTime.wYear  &&
       stCurTime.wMonth==stAlarmTime.wMonth &&
       stCurTime.wDay  ==stAlarmTime.wDay) // math all condition
    {
        isAlarm = TRUE;
        // We don't use alarm from now, disable it
        SETREG8(&pRTCRegs->CMD_REG, RTC_CMDREG_TDM);
    }

    // Read alarm register to acknowledge interrupt request
    INREG8(&pRTCRegs->AL_MINTL);
    INREG8(&pRTCRegs->AL_HOUR);
    INREG8(&pRTCRegs->AL_DAY);

    return isAlarm;
}

//------------------------------------------------------------------------------
//
//  Function:  DS_SetAlarmTime
//
//  This function is called by OEMSetAlarmTime
//
BOOL DS_SetAlarmTime(LPSYSTEMTIME lpst) 
{
    DS1386_RTC_REGS *pRTCRegs = OALPAtoUA(BSP_REG_PA_NVRAM);

    OALMSG(OAL_RTC&&OAL_FUNC, (L"+OEMSetAlarmTime()\r\n"));

    if(lpst->wYear<2000 ||lpst->wYear>=2100)
        return FALSE;

    // Save alarm time for later
    stAlarmTime = *lpst;

    // Disable Time-Of-Day Alarm
    SETREG8(&pRTCRegs->CMD_REG, RTC_CMDREG_TDM);

    do {
        // Convert and populate RTC registers with alarm values
        OUTREG8(&pRTCRegs->AL_MINTL, DecimalToRegVal(lpst->wMinute));
        OUTREG8(&pRTCRegs->AL_HOUR,  DecimalToRegVal(lpst->wHour));
        OUTREG8(&pRTCRegs->AL_DAY,   DecimalToRegVal(lpst->wDayOfWeek+1)); // DS start from 1, but lpst from 0
    } while(lpst->wMinute   !=RegValToDecimal(INREG8(&pRTCRegs->AL_MINTL))||
            lpst->wHour     !=RegValToDecimal(INREG8(&pRTCRegs->AL_HOUR)) ||
            lpst->wDayOfWeek!=RegValToDecimal(INREG8(&pRTCRegs->AL_DAY)-1));

    // Re-enable alarm interrupt
    CLRREG8(&pRTCRegs->CMD_REG, RTC_CMDREG_TDM);

    OALMSG(OAL_RTC&&OAL_FUNC, (L"-OEMSetAlarmTime(rc = %d)\r\n", TRUE));
    return TRUE;
}

//------------------------------------------------------------------------------
//
//  Function:  DS_GetRealTime
//
//  This function is called by OEMGetRealTime
//
BOOL DS_GetRealTime(LPSYSTEMTIME lpst) 
{
    DS1386_RTC_REGS *pRTCRegs = OALPAtoUA(BSP_REG_PA_NVRAM);
    UINT8 yearReg;

    OALMSG(OAL_RTC&&OAL_FUNC, (L"+OEMGetRealTime\r\n"));

    // Set TE bit to enable transfer between external and internal clock registers
    OUTREG8(&pRTCRegs->CMD_REG, (INREG8(&pRTCRegs->CMD_REG) | RTC_CMDREG_TE));

    do {
    // Convert and populate SYSTEMTIME values
    lpst->wMilliseconds = 0;
    lpst->wSecond       = RegValToDecimal(INREG8(&pRTCRegs->SECTL));
    lpst->wMinute       = RegValToDecimal(INREG8(&pRTCRegs->MINTL));
    lpst->wHour         = RegValToDecimal(INREG8(&pRTCRegs->HOURS));
    lpst->wDayOfWeek    = INREG8(&pRTCRegs->DAYS)-1; // DS start from 1, but lpst from 0
    lpst->wDay          = RegValToDecimal(INREG8(&pRTCRegs->DATE));
    lpst->wMonth        = RegValToDecimal( INREG8(&pRTCRegs->MONTH) & RTC_MReg_DIGIT );
    yearReg = (UINT8)RegValToDecimal(INREG8(&pRTCRegs->YEARS));
    lpst->wYear         = 2000 + yearReg;
    } while( ( lpst->wSecond!=RegValToDecimal(INREG8(&pRTCRegs->SECTL)) )||( lpst->wMinute!=RegValToDecimal(INREG8(&pRTCRegs->MINTL)) ) );

    OALMSG(OAL_RTC&&OAL_FUNC, (L"-OEMGetRealTime\r\n"));

    return TRUE;
}

//------------------------------------------------------------------------------
//
//  Function:  DS_SetRealTime
//
//  This function is called by OEMSetRealTime
//
BOOL DS_SetRealTime(LPSYSTEMTIME lpst) 
{
    DS1386_RTC_REGS *pRTCRegs = OALPAtoUA(BSP_REG_PA_NVRAM);
    WORD year;
    UINT8 hour, month;

    OALMSG(OAL_RTC&&OAL_FUNC, (L"+OEMSetRealTime()\r\n"));

    // Set TE bit to enable transfer between external and internal clock registers
    OUTREG8(&pRTCRegs->CMD_REG, (INREG8(&pRTCRegs->CMD_REG) | RTC_CMDREG_TE));

    year = lpst->wYear;
    if( (year <= 2099)&&(year>=2000) )
		year = year%100;
    else
		return FALSE;

    hour = DecimalToRegVal(lpst->wHour)&RTC_HReg_DIGIT;
    month = DecimalToRegVal(lpst->wMonth)&RTC_MReg_DIGIT;

    do {
   // Convert and set SYSTEMTIME into RTC registers
    OUTREG8(&pRTCRegs->SECTL, DecimalToRegVal(lpst->wSecond));
    OUTREG8(&pRTCRegs->MINTL, DecimalToRegVal(lpst->wMinute));
    OUTREG8(&pRTCRegs->HOURS, (hour & ~RTC_24H_MODE) ); // set to be 24H format
    OUTREG8(&pRTCRegs->DAYS,  (UINT8)(lpst->wDayOfWeek+1)); // DS start from 1, but lpst from 0
    OUTREG8(&pRTCRegs->DATE, DecimalToRegVal(lpst->wDay));
    OUTREG8(&pRTCRegs->MONTH, month);
    OUTREG8(&pRTCRegs->YEARS, DecimalToRegVal((UINT8)year));
    } while( ( lpst->wSecond!=RegValToDecimal(INREG8(&pRTCRegs->SECTL)) )||( lpst->wMinute!=RegValToDecimal(INREG8(&pRTCRegs->MINTL)) ) );

    // Enable RTC oscillator
    OUTREG8(&pRTCRegs->MONTH, (INREG8(&pRTCRegs->MONTH) & ~RTC_EOSC));

    OALMSG(OAL_RTC&&OAL_FUNC, (L"-OEMSetRealTime(rc = %d)\r\n", TRUE));

    return TRUE;
}


//------------------------------------------------------------------------------
//
//  Function:   RegValtoDecimal 
//
UINT16 RegValToDecimal(UINT8 regVal)
{
    UINT16 tensVal = 0;
    UINT16 onesVal = 0;

    onesVal = regVal & 0x0F;
    tensVal = ((regVal >> 4) & 0x0F) * 10;

    return (tensVal + onesVal);
}


//------------------------------------------------------------------------------
//
//  Function:   DecimalToRegVal
//
UINT8 DecimalToRegVal(UINT16 decimal)
{
    UINT8 tensVal = 0;
    UINT8 onesVal = 0;

    tensVal = ((decimal / 10) << 4) & 0xF0;
    onesVal = (decimal % 10);

    return (tensVal | onesVal);
}


//------------------------------------------------------------------------------

