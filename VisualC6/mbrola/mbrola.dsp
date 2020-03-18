# Microsoft Developer Studio Project File - Name="mbrola" - Package Owner=<4>
# Microsoft Developer Studio Generated Build File, Format Version 6.00
# ** DO NOT EDIT **

# TARGTYPE "Win32 (x86) Console Application" 0x0103

CFG=mbrola - Win32 Debug
!MESSAGE This is not a valid makefile. To build this project using NMAKE,
!MESSAGE use the Export Makefile command and run
!MESSAGE 
!MESSAGE NMAKE /f "mbrola.mak".
!MESSAGE 
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

# Begin Project
# PROP AllowPerConfigDependencies 0
# PROP Scc_ProjName ""
# PROP Scc_LocalPath ""
CPP=cl.exe
RSC=rc.exe

!IF  "$(CFG)" == "mbrola - Win32 Release"

# PROP BASE Use_MFC 0
# PROP BASE Use_Debug_Libraries 0
# PROP BASE Output_Dir "Release"
# PROP BASE Intermediate_Dir "Release"
# PROP BASE Target_Dir ""
# PROP Use_MFC 0
# PROP Use_Debug_Libraries 0
# PROP Output_Dir "Release"
# PROP Intermediate_Dir "Release"
# PROP Target_Dir ""
# ADD BASE CPP /nologo /W3 /GX /O2 /D "WIN32" /D "NDEBUG" /D "_CONSOLE" /D "_MBCS" /YX /FD /c
# ADD CPP /nologo /W3 /GX /O2 /I "..\..\Standalone" /I "..\..\Database" /I "..\..\Engine" /I "..\..\Misc" /I "..\..\Parser" /D "WIN32" /D "NDEBUG" /D "_CONSOLE" /D "_MBCS" /D "_CRT_SECURE_NO_WARNINGS" /D "TARGET_OS_DOS" /D "LITTLE_ENDIAN" /YX /FD /c
# ADD BASE RSC /l 0x3809 /d "NDEBUG"
# ADD RSC /l 0x3809 /d "NDEBUG"
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LINK32=link.exe
# ADD BASE LINK32 kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib /nologo /subsystem:console /machine:I386
# ADD LINK32 kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib /nologo /subsystem:console /machine:I386

!ELSEIF  "$(CFG)" == "mbrola - Win32 Debug"

# PROP BASE Use_MFC 0
# PROP BASE Use_Debug_Libraries 1
# PROP BASE Output_Dir "Debug"
# PROP BASE Intermediate_Dir "Debug"
# PROP BASE Target_Dir ""
# PROP Use_MFC 0
# PROP Use_Debug_Libraries 1
# PROP Output_Dir "Debug"
# PROP Intermediate_Dir "Debug"
# PROP Target_Dir ""
# ADD BASE CPP /nologo /W3 /Gm /GX /ZI /Od /D "WIN32" /D "_DEBUG" /D "_CONSOLE" /D "_MBCS" /YX /FD /GZ /c
# ADD CPP /nologo /W3 /Gm /GX /ZI /Od /I "..\..\Standalone" /I "..\..\Database" /I "..\..\Engine" /I "..\..\Misc" /I "..\..\Parser" /D "WIN32" /D "_DEBUG" /D "_CONSOLE" /D "_MBCS" /D "_CRT_SECURE_NO_WARNINGS" /D "TARGET_OS_DOS" /D "LITTLE_ENDIAN" /YX /FD /GZ /c
# ADD BASE RSC /l 0x3809 /d "_DEBUG"
# ADD RSC /l 0x3809 /d "_DEBUG"
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LINK32=link.exe
# ADD BASE LINK32 kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib /nologo /subsystem:console /debug /machine:I386 /pdbtype:sept
# ADD LINK32 kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib /nologo /subsystem:console /debug /machine:I386 /pdbtype:sept

!ENDIF 

# Begin Target

# Name "mbrola - Win32 Release"
# Name "mbrola - Win32 Debug"
# Begin Group "Source Files"

# PROP Default_Filter "cpp;c;cxx;rc;def;r;odl;idl;hpj;bat"
# Begin Source File

SOURCE=..\..\Misc\audio.c
# End Source File
# Begin Source File

SOURCE=..\..\Misc\common.c
# End Source File
# Begin Source File

SOURCE=..\..\Database\database.c
# End Source File
# Begin Source File

SOURCE=..\..\Database\database_old.c
# End Source File
# Begin Source File

SOURCE=..\..\Engine\diphone.c
# End Source File
# Begin Source File

SOURCE=..\..\Database\diphone_info.c
# End Source File
# Begin Source File

SOURCE=..\..\Parser\fifo.c
# End Source File
# Begin Source File

SOURCE=..\..\Misc\g711.c
# End Source File
# Begin Source File

SOURCE=..\..\Database\hash_tab.c
# End Source File
# Begin Source File

SOURCE=..\..\Parser\input_fifo.c
# End Source File
# Begin Source File

SOURCE=..\..\Parser\input_file.c
# End Source File
# Begin Source File

SOURCE=..\..\Database\little_big.c
# End Source File
# Begin Source File

SOURCE=..\..\Misc\mbralloc.c
# End Source File
# Begin Source File

SOURCE=..\..\Engine\mbrola.c
# End Source File
# Begin Source File

SOURCE=..\..\Parser\parser_input.c
# End Source File
# Begin Source File

SOURCE=..\..\Parser\phonbuff.c
# End Source File
# Begin Source File

SOURCE=..\..\Parser\phone.c
# End Source File
# Begin Source File

SOURCE=..\..\Database\rom_database.c
# End Source File
# Begin Source File

SOURCE=..\..\Database\rom_handling.c
# End Source File
# Begin Source File

SOURCE=..\..\Standalone\synth.c
# End Source File
# Begin Source File

SOURCE=..\..\Misc\vp_error.c
# End Source File
# Begin Source File

SOURCE=..\..\Database\zstring_list.c
# End Source File
# End Group
# Begin Group "Header Files"

# PROP Default_Filter "h;hpp;hxx;hm;inl"
# Begin Source File

SOURCE=..\..\Standalone\synth.h
# End Source File
# End Group
# Begin Group "Resource Files"

# PROP Default_Filter "ico;cur;bmp;dlg;rc2;rct;bin;rgs;gif;jpg;jpeg;jpe"
# End Group
# End Target
# End Project
