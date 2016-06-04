# Microsoft Developer Studio Generated NMAKE File, Based on GRP.dsp
!IF "$(CFG)" == ""
CFG=GRP - Win32 Debug
!MESSAGE No configuration specified. Defaulting to GRP - Win32 Debug.
!ENDIF 

!IF "$(CFG)" != "GRP - Win32 Release" && "$(CFG)" != "GRP - Win32 Debug"
!MESSAGE Invalid configuration "$(CFG)" specified.
!MESSAGE You can specify a configuration when running NMAKE
!MESSAGE by defining the macro CFG on the command line. For example:
!MESSAGE 
!MESSAGE NMAKE /f "GRP.mak" CFG="GRP - Win32 Debug"
!MESSAGE 
!MESSAGE Possible choices for configuration are:
!MESSAGE 
!MESSAGE "GRP - Win32 Release" (based on "Win32 (x86) Dynamic-Link Library")
!MESSAGE "GRP - Win32 Debug" (based on "Win32 (x86) Dynamic-Link Library")
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

!IF  "$(CFG)" == "GRP - Win32 Release"

OUTDIR=.\Release
INTDIR=.\Release
# Begin Custom Macros
OutDir=.\Release
# End Custom Macros

ALL : "$(OUTDIR)\rip_GRP.rip"


CLEAN :
	-@erase "$(INTDIR)\GRP.obj"
	-@erase "$(INTDIR)\GRP.pch"
	-@erase "$(INTDIR)\StdAfx.obj"
	-@erase "$(INTDIR)\vc60.idb"
	-@erase "$(OUTDIR)\rip_GRP.exp"
	-@erase "$(OUTDIR)\rip_GRP.rip"

"$(OUTDIR)" :
    if not exist "$(OUTDIR)/$(NULL)" mkdir "$(OUTDIR)"

CPP_PROJ=/nologo /MT /W3 /GX /O2 /D "WIN32" /D "NDEBUG" /D "_WINDOWS" /D "_MBCS" /D "_USRDLL" /D "GRP_EXPORTS" /Fp"$(INTDIR)\GRP.pch" /Yu"stdafx.h" /Fo"$(INTDIR)\\" /Fd"$(INTDIR)\\" /FD /c 
MTL_PROJ=/nologo /D "NDEBUG" /mktyplib203 /win32 
BSC32=bscmake.exe
BSC32_FLAGS=/nologo /o"$(OUTDIR)\GRP.bsc" 
BSC32_SBRS= \
	
LINK32=link.exe
LINK32_FLAGS=msvcrt.lib kernel32.lib /nologo /dll /incremental:no /pdb:"$(OUTDIR)\rip_GRP.pdb" /machine:I386 /nodefaultlib /def:".\GRP.def" /out:"$(OUTDIR)\rip_GRP.rip" /implib:"$(OUTDIR)\rip_GRP.lib" /opt:nowin98 
DEF_FILE= \
	".\GRP.def"
LINK32_OBJS= \
	"$(INTDIR)\GRP.obj" \
	"$(INTDIR)\StdAfx.obj"

"$(OUTDIR)\rip_GRP.rip" : "$(OUTDIR)" $(DEF_FILE) $(LINK32_OBJS)
    $(LINK32) @<<
  $(LINK32_FLAGS) $(LINK32_OBJS)
<<

!ELSEIF  "$(CFG)" == "GRP - Win32 Debug"

OUTDIR=.\Debug
INTDIR=.\Debug
# Begin Custom Macros
OutDir=.\Debug
# End Custom Macros

ALL : "$(OUTDIR)\rip_GRP.rip"


CLEAN :
	-@erase "$(INTDIR)\GRP.obj"
	-@erase "$(INTDIR)\GRP.pch"
	-@erase "$(INTDIR)\StdAfx.obj"
	-@erase "$(INTDIR)\vc60.idb"
	-@erase "$(INTDIR)\vc60.pdb"
	-@erase "$(OUTDIR)\rip_GRP.exp"
	-@erase "$(OUTDIR)\rip_GRP.ilk"
	-@erase "$(OUTDIR)\rip_GRP.lib"
	-@erase "$(OUTDIR)\rip_GRP.pdb"
	-@erase "$(OUTDIR)\rip_GRP.rip"

"$(OUTDIR)" :
    if not exist "$(OUTDIR)/$(NULL)" mkdir "$(OUTDIR)"

CPP_PROJ=/nologo /MTd /W3 /Gm /GX /ZI /Od /D "WIN32" /D "_DEBUG" /D "_WINDOWS" /D "_MBCS" /D "_USRDLL" /D "GRP_EXPORTS" /Fp"$(INTDIR)\GRP.pch" /Yu"stdafx.h" /Fo"$(INTDIR)\\" /Fd"$(INTDIR)\\" /FD /GZ /c 
MTL_PROJ=/nologo /D "_DEBUG" /mktyplib203 /win32 
BSC32=bscmake.exe
BSC32_FLAGS=/nologo /o"$(OUTDIR)\GRP.bsc" 
BSC32_SBRS= \
	
LINK32=link.exe
LINK32_FLAGS=kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib /nologo /dll /incremental:yes /pdb:"$(OUTDIR)\rip_GRP.pdb" /debug /machine:I386 /def:".\GRP.def" /out:"$(OUTDIR)\rip_GRP.rip" /implib:"$(OUTDIR)\rip_GRP.lib" /pdbtype:sept 
DEF_FILE= \
	".\GRP.def"
LINK32_OBJS= \
	"$(INTDIR)\GRP.obj" \
	"$(INTDIR)\StdAfx.obj"

"$(OUTDIR)\rip_GRP.rip" : "$(OUTDIR)" $(DEF_FILE) $(LINK32_OBJS)
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
!IF EXISTS("GRP.dep")
!INCLUDE "GRP.dep"
!ELSE 
!MESSAGE Warning: cannot find "GRP.dep"
!ENDIF 
!ENDIF 


!IF "$(CFG)" == "GRP - Win32 Release" || "$(CFG)" == "GRP - Win32 Debug"
SOURCE=.\GRP.cpp

"$(INTDIR)\GRP.obj" : $(SOURCE) "$(INTDIR)" "$(INTDIR)\GRP.pch"


SOURCE=.\StdAfx.cpp

!IF  "$(CFG)" == "GRP - Win32 Release"

CPP_SWITCHES=/nologo /MT /W3 /GX /O2 /D "WIN32" /D "NDEBUG" /D "_WINDOWS" /D "_MBCS" /D "_USRDLL" /D "GRP_EXPORTS" /Fp"$(INTDIR)\GRP.pch" /Yc"stdafx.h" /Fo"$(INTDIR)\\" /Fd"$(INTDIR)\\" /FD /c 

"$(INTDIR)\StdAfx.obj"	"$(INTDIR)\GRP.pch" : $(SOURCE) "$(INTDIR)"
	$(CPP) @<<
  $(CPP_SWITCHES) $(SOURCE)
<<


!ELSEIF  "$(CFG)" == "GRP - Win32 Debug"

CPP_SWITCHES=/nologo /MTd /W3 /Gm /GX /ZI /Od /D "WIN32" /D "_DEBUG" /D "_WINDOWS" /D "_MBCS" /D "_USRDLL" /D "GRP_EXPORTS" /Fp"$(INTDIR)\GRP.pch" /Yc"stdafx.h" /Fo"$(INTDIR)\\" /Fd"$(INTDIR)\\" /FD /GZ /c 

"$(INTDIR)\StdAfx.obj"	"$(INTDIR)\GRP.pch" : $(SOURCE) "$(INTDIR)"
	$(CPP) @<<
  $(CPP_SWITCHES) $(SOURCE)
<<


!ENDIF 


!ENDIF 

