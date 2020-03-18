# Microsoft Developer Studio Generated NMAKE File, Based on mbrola.dsp
!IF "$(CFG)" == ""
CFG=mbrola - Win32 Debug
!MESSAGE No configuration specified. Defaulting to mbrola - Win32 Debug.
!ENDIF 

!IF "$(CFG)" != "mbrola - Win32 Release" && "$(CFG)" != "mbrola - Win32 Debug"
!MESSAGE Invalid configuration "$(CFG)" specified.
!MESSAGE You can specify a configuration when running NMAKE
!MESSAGE by defining the macro CFG on the command line. For example:
!MESSAGE 
!MESSAGE NMAKE /f "mbrola.mak" CFG="mbrola - Win32 Debug"
!MESSAGE 
!MESSAGE Possible choices for configuration are:
!MESSAGE 
!MESSAGE "mbrola - Win32 Release" (based on "Win32 (x86) Console Application")
!MESSAGE "mbrola - Win32 Debug" (based on "Win32 (x86) Console Application")
!MESSAGE 
!ERROR An invalid configuration is specified.
!ENDIF 

!IF "$(OS)" == "Windows_NT"
NULL=
!ELSE 
NULL=nul
!ENDIF 

CPP=cl.exe
RSC=rc.exe

!IF  "$(CFG)" == "mbrola - Win32 Release"

OUTDIR=.\Release
INTDIR=.\Release
# Begin Custom Macros
OutDir=.\Release
# End Custom Macros

ALL : "$(OUTDIR)\mbrola.exe"


CLEAN :
	-@erase "$(INTDIR)\audio.obj"
	-@erase "$(INTDIR)\common.obj"
	-@erase "$(INTDIR)\database.obj"
	-@erase "$(INTDIR)\database_old.obj"
	-@erase "$(INTDIR)\diphone.obj"
	-@erase "$(INTDIR)\diphone_info.obj"
	-@erase "$(INTDIR)\fifo.obj"
	-@erase "$(INTDIR)\g711.obj"
	-@erase "$(INTDIR)\hash_tab.obj"
	-@erase "$(INTDIR)\input_fifo.obj"
	-@erase "$(INTDIR)\input_file.obj"
	-@erase "$(INTDIR)\little_big.obj"
	-@erase "$(INTDIR)\mbralloc.obj"
	-@erase "$(INTDIR)\mbrola.obj"
	-@erase "$(INTDIR)\parser_input.obj"
	-@erase "$(INTDIR)\phonbuff.obj"
	-@erase "$(INTDIR)\phone.obj"
	-@erase "$(INTDIR)\rom_database.obj"
	-@erase "$(INTDIR)\rom_handling.obj"
	-@erase "$(INTDIR)\synth.obj"
	-@erase "$(INTDIR)\vc60.idb"
	-@erase "$(INTDIR)\vp_error.obj"
	-@erase "$(INTDIR)\zstring_list.obj"
	-@erase "$(OUTDIR)\mbrola.exe"

"$(OUTDIR)" :
    if not exist "$(OUTDIR)/$(NULL)" mkdir "$(OUTDIR)"

CPP_PROJ=/nologo /ML /W3 /GX /O2 /I "..\..\Standalone" /I "..\..\Database" /I "..\..\Engine" /I "..\..\Misc" /I "..\..\Parser" /D "WIN32" /D "NDEBUG" /D "_CONSOLE" /D "_MBCS" /D "_CRT_SECURE_NO_WARNINGS" /D "TARGET_OS_DOS" /D "LITTLE_ENDIAN" /Fp"$(INTDIR)\mbrola.pch" /YX /Fo"$(INTDIR)\\" /Fd"$(INTDIR)\\" /FD /c 
BSC32=bscmake.exe
BSC32_FLAGS=/nologo /o"$(OUTDIR)\mbrola.bsc" 
BSC32_SBRS= \
	
LINK32=link.exe
LINK32_FLAGS=kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib /nologo /subsystem:console /incremental:no /pdb:"$(OUTDIR)\mbrola.pdb" /machine:I386 /out:"$(OUTDIR)\mbrola.exe" 
LINK32_OBJS= \
	"$(INTDIR)\audio.obj" \
	"$(INTDIR)\common.obj" \
	"$(INTDIR)\database.obj" \
	"$(INTDIR)\database_old.obj" \
	"$(INTDIR)\diphone.obj" \
	"$(INTDIR)\diphone_info.obj" \
	"$(INTDIR)\fifo.obj" \
	"$(INTDIR)\g711.obj" \
	"$(INTDIR)\hash_tab.obj" \
	"$(INTDIR)\input_fifo.obj" \
	"$(INTDIR)\input_file.obj" \
	"$(INTDIR)\little_big.obj" \
	"$(INTDIR)\mbralloc.obj" \
	"$(INTDIR)\mbrola.obj" \
	"$(INTDIR)\parser_input.obj" \
	"$(INTDIR)\phonbuff.obj" \
	"$(INTDIR)\phone.obj" \
	"$(INTDIR)\rom_database.obj" \
	"$(INTDIR)\rom_handling.obj" \
	"$(INTDIR)\synth.obj" \
	"$(INTDIR)\vp_error.obj" \
	"$(INTDIR)\zstring_list.obj"

"$(OUTDIR)\mbrola.exe" : "$(OUTDIR)" $(DEF_FILE) $(LINK32_OBJS)
    $(LINK32) @<<
  $(LINK32_FLAGS) $(LINK32_OBJS)
<<

!ELSEIF  "$(CFG)" == "mbrola - Win32 Debug"

OUTDIR=.\Debug
INTDIR=.\Debug
# Begin Custom Macros
OutDir=.\Debug
# End Custom Macros

ALL : "$(OUTDIR)\mbrola.exe"


CLEAN :
	-@erase "$(INTDIR)\audio.obj"
	-@erase "$(INTDIR)\common.obj"
	-@erase "$(INTDIR)\database.obj"
	-@erase "$(INTDIR)\database_old.obj"
	-@erase "$(INTDIR)\diphone.obj"
	-@erase "$(INTDIR)\diphone_info.obj"
	-@erase "$(INTDIR)\fifo.obj"
	-@erase "$(INTDIR)\g711.obj"
	-@erase "$(INTDIR)\hash_tab.obj"
	-@erase "$(INTDIR)\input_fifo.obj"
	-@erase "$(INTDIR)\input_file.obj"
	-@erase "$(INTDIR)\little_big.obj"
	-@erase "$(INTDIR)\mbralloc.obj"
	-@erase "$(INTDIR)\mbrola.obj"
	-@erase "$(INTDIR)\parser_input.obj"
	-@erase "$(INTDIR)\phonbuff.obj"
	-@erase "$(INTDIR)\phone.obj"
	-@erase "$(INTDIR)\rom_database.obj"
	-@erase "$(INTDIR)\rom_handling.obj"
	-@erase "$(INTDIR)\synth.obj"
	-@erase "$(INTDIR)\vc60.idb"
	-@erase "$(INTDIR)\vc60.pdb"
	-@erase "$(INTDIR)\vp_error.obj"
	-@erase "$(INTDIR)\zstring_list.obj"
	-@erase "$(OUTDIR)\mbrola.exe"
	-@erase "$(OUTDIR)\mbrola.ilk"
	-@erase "$(OUTDIR)\mbrola.pdb"

"$(OUTDIR)" :
    if not exist "$(OUTDIR)/$(NULL)" mkdir "$(OUTDIR)"

CPP_PROJ=/nologo /MLd /W3 /Gm /GX /ZI /Od /I "..\..\Standalone" /I "..\..\Database" /I "..\..\Engine" /I "..\..\Misc" /I "..\..\Parser" /D "WIN32" /D "_DEBUG" /D "_CONSOLE" /D "_MBCS" /D "_CRT_SECURE_NO_WARNINGS" /D "TARGET_OS_DOS" /D "LITTLE_ENDIAN" /Fp"$(INTDIR)\mbrola.pch" /YX /Fo"$(INTDIR)\\" /Fd"$(INTDIR)\\" /FD /GZ /c 
BSC32=bscmake.exe
BSC32_FLAGS=/nologo /o"$(OUTDIR)\mbrola.bsc" 
BSC32_SBRS= \
	
LINK32=link.exe
LINK32_FLAGS=kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib /nologo /subsystem:console /incremental:yes /pdb:"$(OUTDIR)\mbrola.pdb" /debug /machine:I386 /out:"$(OUTDIR)\mbrola.exe" /pdbtype:sept 
LINK32_OBJS= \
	"$(INTDIR)\audio.obj" \
	"$(INTDIR)\common.obj" \
	"$(INTDIR)\database.obj" \
	"$(INTDIR)\database_old.obj" \
	"$(INTDIR)\diphone.obj" \
	"$(INTDIR)\diphone_info.obj" \
	"$(INTDIR)\fifo.obj" \
	"$(INTDIR)\g711.obj" \
	"$(INTDIR)\hash_tab.obj" \
	"$(INTDIR)\input_fifo.obj" \
	"$(INTDIR)\input_file.obj" \
	"$(INTDIR)\little_big.obj" \
	"$(INTDIR)\mbralloc.obj" \
	"$(INTDIR)\mbrola.obj" \
	"$(INTDIR)\parser_input.obj" \
	"$(INTDIR)\phonbuff.obj" \
	"$(INTDIR)\phone.obj" \
	"$(INTDIR)\rom_database.obj" \
	"$(INTDIR)\rom_handling.obj" \
	"$(INTDIR)\synth.obj" \
	"$(INTDIR)\vp_error.obj" \
	"$(INTDIR)\zstring_list.obj"

"$(OUTDIR)\mbrola.exe" : "$(OUTDIR)" $(DEF_FILE) $(LINK32_OBJS)
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
!IF EXISTS("mbrola.dep")
!INCLUDE "mbrola.dep"
!ELSE 
!MESSAGE Warning: cannot find "mbrola.dep"
!ENDIF 
!ENDIF 


!IF "$(CFG)" == "mbrola - Win32 Release" || "$(CFG)" == "mbrola - Win32 Debug"
SOURCE=..\..\Misc\audio.c

"$(INTDIR)\audio.obj" : $(SOURCE) "$(INTDIR)"
	$(CPP) $(CPP_PROJ) $(SOURCE)


SOURCE=..\..\Misc\common.c

"$(INTDIR)\common.obj" : $(SOURCE) "$(INTDIR)"
	$(CPP) $(CPP_PROJ) $(SOURCE)


SOURCE=..\..\Database\database.c

"$(INTDIR)\database.obj" : $(SOURCE) "$(INTDIR)"
	$(CPP) $(CPP_PROJ) $(SOURCE)


SOURCE=..\..\Database\database_old.c

"$(INTDIR)\database_old.obj" : $(SOURCE) "$(INTDIR)"
	$(CPP) $(CPP_PROJ) $(SOURCE)


SOURCE=..\..\Engine\diphone.c

"$(INTDIR)\diphone.obj" : $(SOURCE) "$(INTDIR)"
	$(CPP) $(CPP_PROJ) $(SOURCE)


SOURCE=..\..\Database\diphone_info.c

"$(INTDIR)\diphone_info.obj" : $(SOURCE) "$(INTDIR)"
	$(CPP) $(CPP_PROJ) $(SOURCE)


SOURCE=..\..\Parser\fifo.c

"$(INTDIR)\fifo.obj" : $(SOURCE) "$(INTDIR)"
	$(CPP) $(CPP_PROJ) $(SOURCE)


SOURCE=..\..\Misc\g711.c

"$(INTDIR)\g711.obj" : $(SOURCE) "$(INTDIR)"
	$(CPP) $(CPP_PROJ) $(SOURCE)


SOURCE=..\..\Database\hash_tab.c

"$(INTDIR)\hash_tab.obj" : $(SOURCE) "$(INTDIR)"
	$(CPP) $(CPP_PROJ) $(SOURCE)


SOURCE=..\..\Parser\input_fifo.c

"$(INTDIR)\input_fifo.obj" : $(SOURCE) "$(INTDIR)"
	$(CPP) $(CPP_PROJ) $(SOURCE)


SOURCE=..\..\Parser\input_file.c

"$(INTDIR)\input_file.obj" : $(SOURCE) "$(INTDIR)"
	$(CPP) $(CPP_PROJ) $(SOURCE)


SOURCE=..\..\Database\little_big.c

"$(INTDIR)\little_big.obj" : $(SOURCE) "$(INTDIR)"
	$(CPP) $(CPP_PROJ) $(SOURCE)


SOURCE=..\..\Misc\mbralloc.c

"$(INTDIR)\mbralloc.obj" : $(SOURCE) "$(INTDIR)"
	$(CPP) $(CPP_PROJ) $(SOURCE)


SOURCE=..\..\Engine\mbrola.c

"$(INTDIR)\mbrola.obj" : $(SOURCE) "$(INTDIR)"
	$(CPP) $(CPP_PROJ) $(SOURCE)


SOURCE=..\..\Parser\parser_input.c

"$(INTDIR)\parser_input.obj" : $(SOURCE) "$(INTDIR)"
	$(CPP) $(CPP_PROJ) $(SOURCE)


SOURCE=..\..\Parser\phonbuff.c

"$(INTDIR)\phonbuff.obj" : $(SOURCE) "$(INTDIR)"
	$(CPP) $(CPP_PROJ) $(SOURCE)


SOURCE=..\..\Parser\phone.c

"$(INTDIR)\phone.obj" : $(SOURCE) "$(INTDIR)"
	$(CPP) $(CPP_PROJ) $(SOURCE)


SOURCE=..\..\Database\rom_database.c

"$(INTDIR)\rom_database.obj" : $(SOURCE) "$(INTDIR)"
	$(CPP) $(CPP_PROJ) $(SOURCE)


SOURCE=..\..\Database\rom_handling.c

"$(INTDIR)\rom_handling.obj" : $(SOURCE) "$(INTDIR)"
	$(CPP) $(CPP_PROJ) $(SOURCE)


SOURCE=..\..\Standalone\synth.c

"$(INTDIR)\synth.obj" : $(SOURCE) "$(INTDIR)"
	$(CPP) $(CPP_PROJ) $(SOURCE)


SOURCE=..\..\Misc\vp_error.c

"$(INTDIR)\vp_error.obj" : $(SOURCE) "$(INTDIR)"
	$(CPP) $(CPP_PROJ) $(SOURCE)


SOURCE=..\..\Database\zstring_list.c

"$(INTDIR)\zstring_list.obj" : $(SOURCE) "$(INTDIR)"
	$(CPP) $(CPP_PROJ) $(SOURCE)



!ENDIF 

