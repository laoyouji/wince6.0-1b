!if 0
Copyright (c) Microsoft Corporation.  All rights reserved.
!endif
!if 0
Use of this sample source code is subject to the terms of the Microsoft
license agreement under which you licensed this sample source code. If
you did not accept the terms of the license agreement, you are not
authorized to use this sample source code. For the terms of the license,
please see the license agreement between you and Microsoft or, if applicable,
see the LICENSE.RTF on your install media or the root of your tools installation.
THE SAMPLE SOURCE CODE IS PROVIDED "AS IS", WITH NO WARRANTIES.
!endif
#
# Take the eboot.exe that resulted from the standard build and generate eboot.bin from it.
#
BootImage:
!IF "$(NOLINK)" == ""
    SET BUILD_SRE=1
    romimage boot.bib
    set BUILD_SRE=
    romimage $(ROMIMAGE_FLAGS) boot.bib
!IF "$(WINCEREL)"=="1"
    copy $(_PLATFORMROOT)\$(_TGTPLAT)\target\$(_TGTCPU)\$(WINCEDEBUG)\boot.* $(_FLATRELEASEDIR)
!ENDIF
!ENDIF        

