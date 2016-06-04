# Microsoft Developer Studio Generated NMAKE File, Based on 0pakset.dsp
!IF "$(CFG)" == ""
CFG=0pakset - Win32 Debug
!MESSAGE No configuration specified. Defaulting to 0pakset - Win32 Debug.
!ENDIF 

!IF "$(CFG)" != "0pakset - Win32 Release" && "$(CFG)" != "0pakset - Win32 Debug"
!MESSAGE Invalid configuration "$(CFG)" specified.
!MESSAGE You can specify a configuration when running NMAKE
!MESSAGE by defining the macro CFG on the command line. For example:
!MESSAGE 
!MESSAGE NMAKE /f "0pakset.mak" CFG="0pakset - Win32 Debug"
!MESSAGE 
!MESSAGE Possible choices for configuration are:
!MESSAGE 
!MESSAGE "0pakset - Win32 Release" (based on "Win32 (x86) Dynamic-Link Library")
!MESSAGE "0pakset - Win32 Debug" (based on "Win32 (x86) Dynamic-Link Library")
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

!IF  "$(CFG)" == "0pakset - Win32 Release"

OUTDIR=.\Release
INTDIR=.\Release
# Begin Custom Macros
OutDir=.\Release
# End Custom Macros

ALL : "$(OUTDIR)\0pakset.rip"


CLEAN :
	-@erase "$(INTDIR)\0pakset.obj"
	-@erase "$(INTDIR)\0pakset.pch"
	-@erase "$(INTDIR)\StdAfx.obj"
	-@erase "$(INTDIR)\vc60.idb"
	-@erase "$(OUTDIR)\0pakset.exp"
	-@erase "$(OUTDIR)\0pakset.rip"

"$(OUTDIR)" :
    if not exist "$(OUTDIR)/$(NULL)" mkdir "$(OUTDIR)"

CPP_PROJ=/nologo /ML /W3 /GX /O2 /D "WIN32" /D "NDEBUG" /D "_WINDOWS" /D "_MBCS" /D "_USRDLL" /D "0pakset_EXPORTS" /Fp"$(INTDIR)\0pakset.pch" /Yu"stdafx.h" /Fo"$(INTDIR)\\" /Fd"$(INTDIR)\\" /FD /c 
MTL_PROJ=/nologo /D "NDEBUG" /mktyplib203 /win32 
BSC32=bscmake.exe
BSC32_FLAGS=/nologo /o"$(OUTDIR)\0pakset.bsc" 
BSC32_SBRS= \
	
LINK32=link.exe
LINK32_FLAGS=msvcrt.lib kernel32.lib /nologo /dll /incremental:no /pdb:"$(OUTDIR)\0pakset.pdb" /machine:I386 /nodefaultlib:"libc.lib" /nodefaultlib /def:".\0pakset.def" /out:"$(OUTDIR)\0pakset.rip" /implib:"$(OUTDIR)\0pakset.lib" /opt:nowin98 
DEF_FILE= \
	".\0pakset.def"
LINK32_OBJS= \
	"$(INTDIR)\0pakset.obj" \
	"$(INTDIR)\StdAfx.obj"

"$(OUTDIR)\0pakset.rip" : "$(OUTDIR)" $(DEF_FILE) $(LINK32_OBJS)
    $(LINK32) @<<
  $(LINK32_FLAGS) $(LINK32_OBJS)
<<

!ELSEIF  "$(CFG)" == "0pakset - Win32 Debug"

OUTDIR=.\Debug
INTDIR=.\Debug
# Begin Custom Macros
OutDir=.\Debug
# End Custom Macros

ALL : "$(OUTDIR)\0pakset.rip"


CLEAN :
	-@erase "$(INTDIR)\0pakset.obj"
	-@erase "$(INTDIR)\0pakset.pch"
	-@erase "$(INTDIR)\StdAfx.obj"
	-@erase "$(INTDIR)\vc60.idb"
	-@erase "$(INTDIR)\vc60.pdb"
	-@erase "$(OUTDIR)\0pakset.exp"
	-@erase "$(OUTDIR)\0pakset.ilk"
	-@erase "$(OUTDIR)\0pakset.lib"
	-@erase "$(OUTDIR)\0pakset.pdb"
	-@erase "$(OUTDIR)\0pakset.rip"

"$(OUTDIR)" :
    if not exist "$(OUTDIR)/$(NULL)" mkdir "$(OUTDIR)"

CPP_PROJ=/nologo /MTd /W3 /Gm /GX /ZI /Od /D "WIN32" /D "_DEBUG" /D "_WINDOWS" /D "_MBCS" /D "_USRDLL" /D "0pakset_EXPORTS" /Fp"$(INTDIR)\0pakset.pch" /Yu"stdafx.h" /Fo"$(INTDIR)\\" /Fd"$(INTDIR)\\" /FD /GZ /c 
MTL_PROJ=/nologo /D "_DEBUG" /mktyplib203 /win32 
BSC32=bscmake.exe
BSC32_FLAGS=/nologo /o"$(OUTDIR)\0pakset.bsc" 
BSC32_SBRS= \
	
LINK32=link.exe
LINK32_FLAGS=kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib /nologo /dll /incremental:yes /pdb:"$(OUTDIR)\0pakset.pdb" /debug /machine:I386 /def:".\0pakset.def" /out:"$(OUTDIR)\0pakset.rip" /implib:"$(OUTDIR)\0pakset.lib" /pdbtype:sept 
DEF_FILE= \
	".\0pakset.def"
LINK32_OBJS= \
	"$(INTDIR)\0pakset.obj" \
	"$(INTDIR)\StdAfx.obj"

"$(OUTDIR)\0pakset.rip" : "$(OUTDIR)" $(DEF_FILE) $(LINK32_OBJS)
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
!IF EXISTS("0pakset.dep")
!INCLUDE "0pakset.dep"
!ELSE 
!MESSAGE Warning: cannot find "0pakset.dep"
!ENDIF 
!ENDIF 


!IF "$(CFG)" == "0pakset - Win32 Release" || "$(CFG)" == "0pakset - Win32 Debug"
SOURCE=.\0pakset.cpp

"$(INTDIR)\0pakset.obj" : $(SOURCE) "$(INTDIR)" "$(INTDIR)\0pakset.pch"


SOURCE=.\StdAfx.cpp

!IF  "$(CFG)" == "0pakset - Win32 Release"

CPP_SWITCHES=/nologo /ML /W3 /GX /O2 /D "WIN32" /D "NDEBUG" /D "_WINDOWS" /D "_MBCS" /D "_USRDLL" /D "0pakset_EXPORTS" /Fp"$(INTDIR)\0pakset.pch" /Yc"stdafx.h" /Fo"$(INTDIR)\\" /Fd"$(INTDIR)\\" /FD /c 

"$(INTDIR)\StdAfx.obj"	"$(INTDIR)\0pakset.pch" : $(SOURCE) "$(INTDIR)"
	$(CPP) @<<
  $(CPP_SWITCHES) $(SOURCE)
<<


!ELSEIF  "$(CFG)" == "0pakset - Win32 Debug"

CPP_SWITCHES=/nologo /MTd /W3 /Gm /GX /ZI /Od /D "WIN32" /D "_DEBUG" /D "_WINDOWS" /D "_MBCS" /D "_USRDLL" /D "0pakset_EXPORTS" /Fp"$(INTDIR)\0pakset.pch" /Yc"stdafx.h" /Fo"$(INTDIR)\\" /Fd"$(INTDIR)\\" /FD /GZ /c 

"$(INTDIR)\StdAfx.obj"	"$(INTDIR)\0pakset.pch" : $(SOURCE) "$(INTDIR)"
	$(CPP) @<<
  $(CPP_SWITCHES) $(SOURCE)
<<


!ENDIF 


!ENDIF 

