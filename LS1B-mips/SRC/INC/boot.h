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
//  Header: boot.h
//
//  This header file is comprised of component header files that defines
//  the standard include hierarchy for the bootloader. It also defines few
//  trivial constant.
//
#ifndef __BOOT_H
#define __BOOT_H

//------------------------------------------------------------------------------

#include <windows.h>
#include <ceddk.h>
#include <blcommon.h>
#include <nkintr.h>

#include <oal.h>

#include <vrc5477.h>
#include "m1535.h"

#include "bsp_base_regs.h"
#include "args.h"
#include "bsp_cfg.h"
#include "image_cfg.h"

//------------------------------------------------------------------------------

#define BOOT_VERSION_MAJOR          2
#define BOOT_VERSION_MINOR          0

//------------------------------------------------------------------------------

extern BOOT_CFG g_bootCfg;
extern OAL_KITL_DEVICE g_bootDevices[];
extern OAL_KITL_DEVICE g_kitlDevices[];

//------------------------------------------------------------------------------

VOID BLMenu();    

UINT32 BLFlashDownload();
UINT32 BLEthDownload(BOOT_CFG *pBootCfg, OAL_KITL_DEVICE *pBootDevices);
VOID   BLEthConfig(BSP_ARGS *pArgs);

//------------------------------------------------------------------------------

#endif
