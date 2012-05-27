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
//  File:  args.h
//
//  This header file defines device structures and constant related to boot
//  configuration. BOOT_CFG structure defines layout of persistent device
//  information. It is used to control boot process. BSP_ARGS structure defines
//  information passed from boot loader to kernel HAL/OAL. Each structure has
//  version field which should be updated each time when structure layout
//  change.
//
#ifndef __ARGS_H
#define __ARGS_H

//------------------------------------------------------------------------------

#include "oal_args.h"
#include "oal_kitl.h"

//------------------------------------------------------------------------------

#define BOOT_CFG_SIGNATURE      'BCFG'
#define BOOT_CFG_VERSION        1

typedef struct {
    UINT32 signature;                   // Structure signature
    UINT32 version;                     // Structure version

    UINT8  deviceId[16];                // Device identification

    DEVICE_LOCATION bootDevLoc;         // Boot device
    DEVICE_LOCATION kitlDevLoc;         // Debug/KITL device

    UINT32 kitlFlags;                   // Debug/KITL mode
    UINT32 ipAddress;
    UINT32 ipMask;
    UINT32 ipRoute;    
} BOOT_CFG;

//------------------------------------------------------------------------------

#define BSP_ARGS_VERSION            1

typedef struct {
    OAL_ARGS_HEADER header;
    UINT8 deviceId[16];                 // Device identification
    OAL_KITL_ARGS kitl;
} BSP_ARGS;

//------------------------------------------------------------------------------

#endif
