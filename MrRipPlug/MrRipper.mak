# Microsoft Developer Studio Generated NMAKE File, Based on MrRipper.dsp
!IF "$(CFG)" == ""
CFG=MrRipper - Win32 Debug
!MESSAGE No configuration specified. Defaulting to MrRipper - Win32 Debug.
!ENDIF 

!IF "$(CFG)" != "MrRipper - Win32 Release" && "$(CFG)" != "MrRipper - Win32 Debug"
!MESSAGE Invalid configuration "$(CFG)" specified.
!MESSAGE You can specify a configuration when running NMAKE
!MESSAGE by defining the macro CFG on the command line. For example:
!MESSAGE 
!MESSAGE NMAKE /f "MrRipper.mak" CFG="MrRipper - Win32 Debug"
!MESSAGE 
!MESSAGE Possible choices for configuration are:
!MESSAGE 
!MESSAGE "MrRipper - Win32 Release" (based on "Win32 (x86) Dynamic-Link Library")
!MESSAGE "MrRipper - Win32 Debug" (based on "Win32 (x86) Dynamic-Link Library")
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

!IF  "$(CFG)" == "MrRipper - Win32 Release"

OUTDIR=.\Release
INTDIR=.\Release
# Begin Custom Macros
OutDir=.\Release
# End Custom Macros

ALL : "$(OUTDIR)\MrRipper.dll"


CLEAN :
	-@erase "$(INTDIR)\MrRipper.obj"
	-@erase "$(INTDIR)\Script1.res"
	-@erase "$(INTDIR)\vc60.idb"
	-@erase "$(OUTDIR)\MrRipper.dll"
	-@erase "$(OUTDIR)\MrRipper.exp"

"$(OUTDIR)" :
    if not exist "$(OUTDIR)/$(NULL)" mkdir "$(OUTDIR)"

CPP_PROJ=/nologo /ML /W3 /GX /O2 /D "WIN32" /D "_WINDOWS" /D "_MBCS" /D "MRRIPPER_EXPORTS" /D "_USRDLL" /Fo"$(INTDIR)\\" /Fd"$(INTDIR)\\" /FD /c 
MTL_PROJ=
RSC_PROJ=/l 0x409 /fo"$(INTDIR)\Script1.res" /d "NDEBUG" 
BSC32=bscmake.exe
BSC32_FLAGS=/nologo /o"$(OUTDIR)\MrRipper.bsc" 
BSC32_SBRS= \
	
LINK32=link.exe
LINK32_FLAGS=kernel32.lib advapi32.lib msvcrt.lib /nologo /dll /pdb:none /machine:I386 /nodefaultlib:"libc.lib" /nodefaultlib /def:".\mrripper.def" /out:"$(OUTDIR)\MrRipper.dll" /implib:"$(OUTDIR)\MrRipper.lib" /opt:nowin98 
DEF_FILE= \
	".\mrripper.def"
LINK32_OBJS= \
	"$(INTDIR)\MrRipper.obj" \
	"$(INTDIR)\Script1.res"

"$(OUTDIR)\MrRipper.dll" : "$(OUTDIR)" $(DEF_FILE) $(LINK32_OBJS)
    $(LINK32) @<<
  $(LINK32_FLAGS) $(LINK32_OBJS)
<<

!ELSEIF  "$(CFG)" == "MrRipper - Win32 Debug"

OUTDIR=.\Debug
INTDIR=.\Debug
# Begin Custom Macros
OutDir=.\Debug
# End Custom Macros

ALL : "$(OUTDIR)\MrRipper.dll"


CLEAN :
	-@erase "$(INTDIR)\MrRipper.obj"
	-@erase "$(INTDIR)\Script1.res"
	-@erase "$(INTDIR)\vc60.idb"
	-@erase "$(INTDIR)\vc60.pdb"
	-@erase "$(OUTDIR)\MrRipper.dll"
	-@erase "$(OUTDIR)\MrRipper.exp"
	-@erase "$(OUTDIR)\MrRipper.ilk"
	-@erase "$(OUTDIR)\MrRipper.lib"
	-@erase "$(OUTDIR)\MrRipper.pdb"

"$(OUTDIR)" :
    if not exist "$(OUTDIR)/$(NULL)" mkdir "$(OUTDIR)"

CPP_PROJ=/nologo /ML /W3 /Gm /GX /ZI /Od /D "WIN32" /D "_DEBUG" /D "_WINDOWS" /D "_MBCS" /D "_USRDLL" /D "MRRIPPER_EXPORTS" /Fo"$(INTDIR)\\" /Fd"$(INTDIR)\\" /FD /GZ /c 
MTL_PROJ=/nologo /D "_DEBUG" /mktyplib203 /win32 
RSC_PROJ=/l 0x419 /fo"$(INTDIR)\Script1.res" /d "_DEBUG" 
BSC32=bscmake.exe
BSC32_FLAGS=/nologo /o"$(OUTDIR)\MrRipper.bsc" 
BSC32_SBRS= \
	
LINK32=link.exe
LINK32_FLAGS=kernel32.lib advapi32.lib libcd.lib /nologo /dll /incremental:yes /pdb:"$(OUTDIR)\MrRipper.pdb" /debug /machine:I386 /def:".\mrripper.def" /out:"$(OUTDIR)\MrRipper.dll" /implib:"$(OUTDIR)\MrRipper.lib" /pdbtype:sept 
DEF_FILE= \
	".\mrripper.def"
LINK32_OBJS= \
	"$(INTDIR)\MrRipper.obj" \
	"$(INTDIR)\Script1.res"

"$(OUTDIR)\MrRipper.dll" : "$(OUTDIR)" $(DEF_FILE) $(LINK32_OBJS)
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
!IF EXISTS("MrRipper.dep")
!INCLUDE "MrRipper.dep"
!ELSE 
!MESSAGE Warning: cannot find "MrRipper.dep"
!ENDIF 
!ENDIF 


!IF "$(CFG)" == "MrRipper - Win32 Release" || "$(CFG)" == "MrRipper - Win32 Debug"
SOURCE=.\MrRipper.cpp

"$(INTDIR)\MrRipper.obj" : $(SOURCE) "$(INTDIR)"


SOURCE=.\Script1.rc

"$(INTDIR)\Script1.res" : $(SOURCE) "$(INTDIR)"
	$(RSC) $(RSC_PROJ) $(SOURCE)



!ENDIF 

