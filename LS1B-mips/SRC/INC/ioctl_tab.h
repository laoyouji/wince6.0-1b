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
//  File:  ioctl_tab.h
//
//  Configuration file for the OAL IOCTL component.
//
//  This file is included by the platform's ioctl.c file and defines the 
//  global IOCTL table, g_oalIoCtlTable[]. Therefore, this file may ONLY
//  define OAL_IOCTL_HANDLER entries. 
//
// IOCTL CODE,                          Flags   Handler Function
//------------------------------------------------------------------------------

//IOCTL for iltiming.exe
#ifdef OAL_ILTIMING
{ IOCTL_HAL_ILTIMING,                       0,  OALIoCtlHalILTiming         },
#endif

{ IOCTL_HAL_POSTINIT,                       0,  OALIoCtlHalPostInit         },

#include <oal_ioctl_tab.h>

// Required Termination
{ 0,                                        0,  NULL                        }

//------------------------------------------------------------------------------
