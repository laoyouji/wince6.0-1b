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
//  Header: ioctl_cfg.h
//
//  This file contains constant used in info OEMIoControl calls
//
#ifndef __IOCTL_CFG_H
#define __IOCTL_CFG_H

#if __cplusplus
extern "C" {
#endif

//------------------------------------------------------------------------------
//
//  Define: IOCTL_PLATFORM_TYPE/OEM
//
//  Defines the platform type and OEM string.
//

#define IOCTL_PLATFORM_TYPE                 L"SG2_VR5500"
#define IOCTL_PLATFORM_OEM                  L"NEC SGear VR5500"

//------------------------------------------------------------------------------
//
//  Define: IOCTL_PROCESSOR_VENDOR/NAME/CORE
//
//  Defines the processor information
//

#define IOCTL_PROCESSOR_VENDOR              L"NEC Corporation"
#define IOCTL_PROCESSOR_NAME                L"Vr5500"
#define IOCTL_PROCESSOR_CORE                L"MIPSIV"

//------------------------------------------------------------------------------
//
//  Define: IOCTL_PROCESSOR_INSTRUCTION_SET
//
//  Defines the processor instruction set information
//

#define IOCTL_PROCESSOR_INSTRUCTION_SET     0
#define IOCTL_PROCESSOR_CLOCK_SPEED         0

//------------------------------------------------------------------------------

#if __cplusplus
}
#endif

#endif 
