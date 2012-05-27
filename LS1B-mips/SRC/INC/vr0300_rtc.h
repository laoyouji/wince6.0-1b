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
//  File:  vr0300_rtc.h
//
//  This header file defines Real Time Clock registers layout and
//  associated constants and types.
//

typedef volatile struct {
	UINT8 SECLL;
	UINT8 SECTL;
	UINT8 MINTL;
	UINT8 AL_MINTL;
	UINT8 HOURS;
	UINT8 AL_HOUR;
	UINT8 DAYS;
	UINT8 AL_DAY;
	UINT8 DATE;
	UINT8 MONTH;
	UINT8 YEARS;
	UINT8 CMD_REG;
	UINT8 AL_WATCHDOG[2];
} DS1386_RTC_REGS, *PDS1386_RTC_REGS;

// Bit field for RTC Command Register
#define RTC_CMDREG_TE 0x80
#define RTC_CMDREG_IPSW 0x40
#define RTC_CMDREG_IBH 0x20
#define RTC_CMDREG_PU  0x10
#define RTC_CMDREG_WAM 0x08
#define RTC_CMDREG_TDM 0x04
#define RTC_CMDREG_WAF 0x02
#define RTC_CMDREG_TDF 0x01

// Bit field for RTC Alarm Mask
#define RTC_ALMIN_ENB 0x80
#define RTC_ALHOR_ENB 0x80
#define RTC_ALDAY_ENB 0x80

//Bit field for Enable-OSC in Month Register
#define RTC_EOSC  0x80

// Bit field for 24-Hour mode in Hours Register
#define RTC_24H_MODE 0x40
#define RTC_AMPM_FLAG 0x20

// Bit field for Hour Register effective bits mask -- 24 hour mode
#define RTC_HReg_DIGIT 0x3F

// Bit field for Month Register effective bits mask
#define RTC_MReg_DIGIT 0x1F

// Memory Map Address for NV RAM on DDB-VR5432A/VR5500A
#define BSP_REG_PA_NVRAM            0x1A000000
//#define BSP_REG_PA_NVRAM            0xBA000000


