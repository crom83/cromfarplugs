# Microsoft Developer Studio Generated NMAKE File, Based on 0wwmix.dsp
!IF "$(CFG)" == ""
CFG=0wwmix - Win32 Debug
!MESSAGE No configuration specified. Defaulting to 0wwmix - Win32 Debug.
!ENDIF 

!IF "$(CFG)" != "0wwmix - Win32 Release" && "$(CFG)" != "0wwmix - Win32 Debug"
!MESSAGE Invalid configuration "$(CFG)" specified.
!MESSAGE You can specify a configuration when running NMAKE
!MESSAGE by defining the macro CFG on the command line. For example:
!MESSAGE 
!MESSAGE NMAKE /f "0wwmix.mak" CFG="0wwmix - Win32 Debug"
!MESSAGE 
!MESSAGE Possible choices for configuration are:
!MESSAGE 
!MESSAGE "0wwmix - Win32 Release" (based on "Win32 (x86) Dynamic-Link Library")
!MESSAGE "0wwmix - Win32 Debug" (based on "Win32 (x86) Dynamic-Link Library")
!MESSAGE 
!ERROR An invalid configuration is specified.
!ENDIF 

!IF "$(OS)" == "Windows_NT"
NULL=
!ELSE 
NULL=nul
!ENDIF 

CPP=cl.exe
MTL=midl.exe
RSC=rc.exe

!IF  "$(CFG)" == "0wwmix - Win32 Release"

OUTDIR=.\Release
INTDIR=.\Release
# Begin Custom Macros
OutDir=.\Release
# End Custom Macros

ALL : "$(OUTDIR)\0wwmix.rip"


CLEAN :
	-@erase "$(INTDIR)\0wwmix.obj"
	-@erase "$(INTDIR)\0wwmix.pch"
	-@erase "$(INTDIR)\blowfish.obj"
	-@erase "$(INTDIR)\mix_decode.obj"
	-@erase "$(INTDIR)\StdAfx.obj"
	-@erase "$(INTDIR)\vc60.idb"
	-@erase "$(OUTDIR)\0wwmix.exp"
	-@erase "$(OUTDIR)\0wwmix.rip"

"$(OUTDIR)" :
    if not exist "$(OUTDIR)/$(NULL)" mkdir "$(OUTDIR)"

CPP_PROJ=/nologo /MT /W3 /GX /O2 /D "WIN32" /D "NDEBUG" /D "_WINDOWS" /D "_MBCS" /D "_USRDLL" /D "0wwmix_EXPORTS" /Fp"$(INTDIR)\0wwmix.pch" /Yu"stdafx.h" /Fo"$(INTDIR)\\" /Fd"$(INTDIR)\\" /FD /c 
MTL_PROJ=/nologo /D "NDEBUG" /mktyplib203 /win32 
BSC32=bscmake.exe
BSC32_FLAGS=/nologo /o"$(OUTDIR)\0wwmix.bsc" 
BSC32_SBRS= \
	
LINK32=link.exe
LINK32_FLAGS=kernel32.lib msvcrt.lib /nologo /dll /incremental:no /pdb:"$(OUTDIR)\0wwmix.pdb" /machine:I386 /nodefaultlib:"libc.lib" /nodefaultlib /def:".\0wwmix.def" /out:"$(OUTDIR)\0wwmix.rip" /implib:"$(OUTDIR)\0wwmix.lib" /opt:nowin98 
DEF_FILE= \
	".\0wwmix.def"
LINK32_OBJS= \
	"$(INTDIR)\0wwmix.obj" \
	"$(INTDIR)\blowfish.obj" \
	"$(INTDIR)\mix_decode.obj" \
	"$(INTDIR)\StdAfx.obj"

"$(OUTDIR)\0wwmix.rip" : "$(OUTDIR)" $(DEF_FILE) $(LINK32_OBJS)
    $(LINK32) @<<
  $(LINK32_FLAGS) $(LINK32_OBJS)
<<

!ELSEIF  "$(CFG)" == "0wwmix - Win32 Debug"

OUTDIR=.\Debug
INTDIR=.\Debug
# Begin Custom Macros
OutDir=.\Debug
# End Custom Macros

ALL : "$(OUTDIR)\0wwmix.rip"


CLEAN :
	-@erase "$(INTDIR)\0wwmix.obj"
	-@erase "$(INTDIR)\0wwmix.pch"
	-@erase "$(INTDIR)\blowfish.obj"
	-@erase "$(INTDIR)\mix_decode.obj"
	-@erase "$(INTDIR)\StdAfx.obj"
	-@erase "$(INTDIR)\vc60.idb"
	-@erase "$(INTDIR)\vc60.pdb"
	-@erase "$(OUTDIR)\0wwmix.exp"
	-@erase "$(OUTDIR)\0wwmix.ilk"
	-@erase "$(OUTDIR)\0wwmix.lib"
	-@erase "$(OUTDIR)\0wwmix.pdb"
	-@erase "$(OUTDIR)\0wwmix.rip"

"$(OUTDIR)" :
    if not exist "$(OUTDIR)/$(NULL)" mkdir "$(OUTDIR)"

CPP_PROJ=/nologo /MTd /W3 /Gm /GX /ZI /Od /D "WIN32" /D "_DEBUG" /D "_WINDOWS" /D "_MBCS" /D "_USRDLL" /D "0wwmix_EXPORTS" /Fp"$(INTDIR)\0wwmix.pch" /Yu"stdafx.h" /Fo"$(INTDIR)\\" /Fd"$(INTDIR)\\" /FD /GZ /c 
MTL_PROJ=/nologo /D "_DEBUG" /mktyplib203 /win32 
BSC32=bscmake.exe
BSC32_FLAGS=/nologo /o"$(OUTDIR)\0wwmix.bsc" 
BSC32_SBRS= \
	
LINK32=link.exe
LINK32_FLAGS=kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib /nologo /dll /incremental:yes /pdb:"$(OUTDIR)\0wwmix.pdb" /debug /machine:I386 /def:".\0wwmix.def" /out:"$(OUTDIR)\0wwmix.rip" /implib:"$(OUTDIR)\0wwmix.lib" /pdbtype:sept 
DEF_FILE= \
	".\0wwmix.def"
LINK32_OBJS= \
	"$(INTDIR)\0wwmix.obj" \
	"$(INTDIR)\blowfish.obj" \
	"$(INTDIR)\mix_decode.obj" \
	"$(INTDIR)\StdAfx.obj"

"$(OUTDIR)\0wwmix.rip" : "$(OUTDIR)" $(DEF_FILE) $(LINK32_OBJS)
    $(LINK32) @<<
  $(LINK32_FLAGS) $(LINK32_OBJS)
<<

!ENDIF 

.c{$(INTDIR)}.obj::
   $(CPP) @<<
   $(CPP_PROJ) $< 
<<

.cpp{$(INTDIR)}.obj::
   $(CPP) @<<
   $(CPP_PROJ) $< 
<<

.cxx{$(INTDIR)}.obj::
   $(CPP) @<<
   $(CPP_PROJ) $< 
<<

.c{$(INTDIR)}.sbr::
   $(CPP) @<<
   $(CPP_PROJ) $< 
<<

.cpp{$(INTDIR)}.sbr::
   $(CPP) @<<
   $(CPP_PROJ) $< 
<<

.cxx{$(INTDIR)}.sbr::
   $(CPP) @<<
   $(CPP_PROJ) $< 
<<


!IF "$(NO_EXTERNAL_DEPS)" != "1"
!IF EXISTS("0wwmix.dep")
!INCLUDE "0wwmix.dep"
!ELSE 
!MESSAGE Warning: cannot find "0wwmix.dep"
!ENDIF 
!ENDIF 


!IF "$(CFG)" == "0wwmix - Win32 Release" || "$(CFG)" == "0wwmix - Win32 Debug"
SOURCE=.\0wwmix.cpp

"$(INTDIR)\0wwmix.obj" : $(SOURCE) "$(INTDIR)" "$(INTDIR)\0wwmix.pch"


SOURCE=.\misc\blowfish.cpp

"$(INTDIR)\blowfish.obj" : $(SOURCE) "$(INTDIR)" "$(INTDIR)\0wwmix.pch"
	$(CPP) $(CPP_PROJ) $(SOURCE)


SOURCE=.\misc\mix_decode.cpp

"$(INTDIR)\mix_decode.obj" : $(SOURCE) "$(INTDIR)" "$(INTDIR)\0wwmix.pch"
	$(CPP) $(CPP_PROJ) $(SOURCE)


SOURCE=.\StdAfx.cpp

!IF  "$(CFG)" == "0wwmix - Win32 Release"

CPP_SWITCHES=/nologo /MT /W3 /GX /O2 /D "WIN32" /D "NDEBUG" /D "_WINDOWS" /D "_MBCS" /D "_USRDLL" /D "0wwmix_EXPORTS" /Fp"$(INTDIR)\0wwmix.pch" /Yc"stdafx.h" /Fo"$(INTDIR)\\" /Fd"$(INTDIR)\\" /FD /c 

"$(INTDIR)\StdAfx.obj"	"$(INTDIR)\0wwmix.pch" : $(SOURCE) "$(INTDIR)"
	$(CPP) @<<
  $(CPP_SWITCHES) $(SOURCE)
<<


!ELSEIF  "$(CFG)" == "0wwmix - Win32 Debug"

CPP_SWITCHES=/nologo /MTd /W3 /Gm /GX /ZI /Od /D "WIN32" /D "_DEBUG" /D "_WINDOWS" /D "_MBCS" /D "_USRDLL" /D "0wwmix_EXPORTS" /Fp"$(INTDIR)\0wwmix.pch" /Yc"stdafx.h" /Fo"$(INTDIR)\\" /Fd"$(INTDIR)\\" /FD /GZ /c 

"$(INTDIR)\StdAfx.obj"	"$(INTDIR)\0wwmix.pch" : $(SOURCE) "$(INTDIR)"
	$(CPP) @<<
  $(CPP_SWITCHES) $(SOURCE)
<<


!ENDIF 


!ENDIF 

