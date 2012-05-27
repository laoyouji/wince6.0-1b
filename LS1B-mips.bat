@REM
@REM Copyright (c) Microsoft Corporation.  All rights reserved.
@REM
@REM
@REM Use of this sample source code is subject to the terms of the Microsoft
@REM license agreement under which you licensed this sample source code. If
@REM you did not accept the terms of the license agreement, you are not
@REM authorized to use this sample source code. For the terms of the license,
@REM please see the license agreement between you and Microsoft or, if applicable,
@REM see the LICENSE.RTF on your install media or the root of your tools installation.
@REM THE SAMPLE SOURCE CODE IS PROVIDED "AS IS", WITH NO WARRANTIES.
@REM

set BSP_NOUSBSER=1
set BSP_NOTOUCH=1
set BSP_NOSIR=1
set BSP_NOCURSOR=

set BSP_DISPLAY_NOP=1
set BSP_DISPLAY_RAGEXL=1
set BSP_WAVEDEV_ENSONIQ=1
set BSP_USB_OHCI=1
set BSP_NIC_NE2000_PCMCIA=1
set BSP_NIC_PCX500_PCMCIA=1
set BSP_NIC_XIRCCE2_PCMCIA=1

set BSP_PCCARD_TIPCCARD=1

if not "%_TGTPROJ%"=="maxdx" goto end_maxdx
    set BSP_WAVEDEV_ES1371=
    set BSP_WAVEDEV_ENSONIQ=1
:end_maxdx

