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
//  File:  image_cfg.h
//
#ifndef __IMAGE_CFG_H
#define __IMAGE_CFG_H

//------------------------------------------------------------------------------
//
//  Defines: IMAGE_RAM_xxx/IMAGE_FLASH_xxx
//
//  This constants specify board ram and flash memory origin and size.
//
#define IMAGE_RAM_PA_START              0x00000000
#define IMAGE_RAM_SIZE                  0x08000000

#define IMAGE_FLASH_PA_START            0x18000000
#define IMAGE_FLASH_SIZE                0x02000000

#define IMAGE_FLASH_PA_BOOT_START       0x19C00000
#define IMAGE_FLASH_BOOT_SIZE           0x00080000

//------------------------------------------------------------------------------
//
//  Defines: IMAGE_SHARE_ARGS_xxx
//
//  This constants specify shared memory area origin and size.
//
#define IMAGE_SHARE_ARGS_PA_START       0x07FFF800
#define IMAGE_SHARE_ARGS_SIZE           0x00000800

//------------------------------------------------------------------------------
//
//  Defines: IMAGE_WINCE_xxx
//
//  This constant specifies WINCE image origin
//
#define IMAGE_WINCE_CODE_PA_START       0x00800000

//------------------------------------------------------------------------------
//
//  Defines: IMAGE_BOOT_xxx
//
//  This constatns specify boot loader image origins and sizes.
//
#define IMAGE_BOOT_FRAM_PA_START        0x00001000
#define IMAGE_BOOT_FRAM_SIZE            0x01FFF000

#define IMAGE_BOOT_CODE_PA_START        0x07F80000
#define IMAGE_BOOT_CODE_SIZE            0x0007F800

#define IMAGE_BOOT_STACK_PA_START       0x07E80000
#define IMAGE_BOOT_STACK_SIZE           0x00010000

#define IMAGE_BOOT_FLASH_PA_START       0x1FC00000
#define IMAGE_BOOT_FLASH_SIZE           0x00060000

#define IMAGE_BOOT_CONFIG_PA_START      0x1FC60000
#define IMAGE_BOOT_CONFIG_SIZE          0x00020000

//------------------------------------------------------------------------------

#endif
