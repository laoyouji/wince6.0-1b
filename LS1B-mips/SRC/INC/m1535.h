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
//  File:  m1535.h
//
#ifndef __M1535_H
#define __M1535_H

//------------------------------------------------------------------------------

typedef struct {
    union {
        UINT8 config;
        UINT8 index;
    };
    UINT8 data;
} M1535_CFG_REGS;


typedef struct {
    union {
        UINT8 ctrl;
        UINT8 data;
    };
    UINT8 mask;
} M1535_PIC_REGS;

//------------------------------------------------------------------------------

#endif
