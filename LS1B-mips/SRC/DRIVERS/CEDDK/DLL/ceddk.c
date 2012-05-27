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
//  File:  ceddk.c
//
#include <windows.h>

//------------------------------------------------------------------------------
//
// Function:     DllMain
//
// Description:  This is standard DLL entry point. It only dissable calls for
//               each thread...
//

BOOL WINAPI DllMain(HANDLE hInstance, DWORD reason, LPVOID pvReserved)
{
   switch (reason) {
   case DLL_PROCESS_ATTACH:
      DisableThreadLibraryCalls((HMODULE)hInstance);
      break;
    }
    return TRUE;
}

//------------------------------------------------------------------------------
