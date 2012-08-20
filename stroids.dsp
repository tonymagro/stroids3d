# Microsoft Developer Studio Project File - Name="stroids" - Package Owner=<4>
# Microsoft Developer Studio Generated Build File, Format Version 6.00
# ** DO NOT EDIT **

# TARGTYPE "Win32 (x86) Application" 0x0101

CFG=stroids - Win32 Debug
!MESSAGE This is not a valid makefile. To build this project using NMAKE,
!MESSAGE use the Export Makefile command and run
!MESSAGE 
!MESSAGE NMAKE /f "stroids.mak".
!MESSAGE 
!MESSAGE You can specify a configuration when running NMAKE
!MESSAGE by defining the macro CFG on the command line. For example:
!MESSAGE 
!MESSAGE NMAKE /f "stroids.mak" CFG="stroids - Win32 Debug"
!MESSAGE 
!MESSAGE Possible choices for configuration are:
!MESSAGE 
!MESSAGE "stroids - Win32 Release" (based on "Win32 (x86) Application")
!MESSAGE "stroids - Win32 Debug" (based on "Win32 (x86) Application")
!MESSAGE 

# Begin Project
# PROP AllowPerConfigDependencies 0
# PROP Scc_ProjName ""
# PROP Scc_LocalPath ""
CPP=cl.exe
MTL=midl.exe
RSC=rc.exe

!IF  "$(CFG)" == "stroids - Win32 Release"

# PROP BASE Use_MFC 0
# PROP BASE Use_Debug_Libraries 0
# PROP BASE Output_Dir "Release"
# PROP BASE Intermediate_Dir "Release"
# PROP BASE Target_Dir ""
# PROP Use_MFC 0
# PROP Use_Debug_Libraries 0
# PROP Output_Dir "Release"
# PROP Intermediate_Dir "Release"
# PROP Ignore_Export_Lib 0
# PROP Target_Dir ""
# ADD BASE CPP /nologo /W3 /GX /O2 /D "WIN32" /D "NDEBUG" /D "_WINDOWS" /D "_MBCS" /YX /FD /c
# ADD CPP /nologo /MD /W3 /GX /O2 /D "WIN32" /D "NDEBUG" /D "_WINDOWS" /D "_MBCS" /YX /FD /c
# ADD BASE MTL /nologo /D "NDEBUG" /mktyplib203 /win32
# ADD MTL /nologo /D "NDEBUG" /mktyplib203 /win32
# ADD BASE RSC /l 0x409 /d "NDEBUG"
# ADD RSC /l 0x409 /d "NDEBUG"
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LINK32=link.exe
# ADD BASE LINK32 kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib /nologo /subsystem:windows /machine:I386
# ADD LINK32 opengl32.lib glu32.lib glaux.lib SDL.lib SDLmain.lib SDL_mixer.lib ws2_32.lib dinput8.lib dxguid.lib /nologo /subsystem:windows /machine:I386

!ELSEIF  "$(CFG)" == "stroids - Win32 Debug"

# PROP BASE Use_MFC 0
# PROP BASE Use_Debug_Libraries 1
# PROP BASE Output_Dir "Debug"
# PROP BASE Intermediate_Dir "Debug"
# PROP BASE Target_Dir ""
# PROP Use_MFC 0
# PROP Use_Debug_Libraries 1
# PROP Output_Dir "Debug"
# PROP Intermediate_Dir "Debug"
# PROP Ignore_Export_Lib 0
# PROP Target_Dir ""
# ADD BASE CPP /nologo /W3 /Gm /GX /ZI /Od /D "WIN32" /D "_DEBUG" /D "_WINDOWS" /D "_MBCS" /YX /FD /GZ /c
# ADD CPP /nologo /MD /W3 /Gm /GX /ZI /Od /D "WIN32" /D "_DEBUG" /D "_WINDOWS" /D "_MBCS" /FR /YX /FD /GZ /c
# ADD BASE MTL /nologo /D "_DEBUG" /mktyplib203 /win32
# ADD MTL /nologo /D "_DEBUG" /mktyplib203 /win32
# ADD BASE RSC /l 0x409 /d "_DEBUG"
# ADD RSC /l 0x409 /d "_DEBUG"
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LINK32=link.exe
# ADD BASE LINK32 kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib /nologo /subsystem:windows /debug /machine:I386 /pdbtype:sept
# ADD LINK32 opengl32.lib glu32.lib glaux.lib SDL.lib SDLmain.lib SDL_mixer.lib ws2_32.lib dinput8.lib dxguid.lib /nologo /subsystem:windows /debug /machine:I386 /out:"stroids.exe" /pdbtype:sept

!ENDIF 

# Begin Target

# Name "stroids - Win32 Release"
# Name "stroids - Win32 Debug"
# Begin Group "Source Files"

# PROP Default_Filter "cpp;c;cxx;rc;def;r;odl;idl;hpj;bat"
# Begin Group "Game"

# PROP Default_Filter ""
# Begin Source File

SOURCE=.\game\e_spacestation.c
# End Source File
# Begin Source File

SOURCE=.\game\g_highscore.c
# End Source File
# Begin Source File

SOURCE=.\game\g_hud.c
# End Source File
# Begin Source File

SOURCE=.\game\g_intro.c
# End Source File
# Begin Source File

SOURCE=.\game\g_keys.c
# End Source File
# Begin Source File

SOURCE=.\game\g_limbo.c
# End Source File
# Begin Source File

SOURCE=.\game\g_main.c
# End Source File
# Begin Source File

SOURCE=.\game\g_menu.c
# End Source File
# Begin Source File

SOURCE=.\game\g_network.c
# End Source File
# Begin Source File

SOURCE=.\game\g_particles.c
# End Source File
# Begin Source File

SOURCE=.\game\g_player.c
# End Source File
# Begin Source File

SOURCE=.\game\g_projectile.c
# End Source File
# Begin Source File

SOURCE=.\game\g_stars.c
# End Source File
# Begin Source File

SOURCE=.\game\g_stroids.c
# End Source File
# End Group
# Begin Source File

SOURCE=.\scommon\keys.c
# End Source File
# Begin Source File

SOURCE=.\scommon\libtarga.c
# End Source File
# Begin Source File

SOURCE=.\scommon\memory.c
# End Source File
# Begin Source File

SOURCE=.\scommon\milkshape.c
# End Source File
# Begin Source File

SOURCE=.\os_sdl\os_sdl.c
# End Source File
# Begin Source File

SOURCE=.\os_sdl\os_sdl_input.c
# End Source File
# Begin Source File

SOURCE=.\ren_gl\ren_draw.c
# End Source File
# Begin Source File

SOURCE=.\ren_gl\ren_gl.c
# End Source File
# Begin Source File

SOURCE=.\ren_gl\ren_model.c
# End Source File
# Begin Source File

SOURCE=.\scommon\s_math.c
# End Source File
# Begin Source File

SOURCE=.\scommon\s_net.c
# End Source File
# Begin Source File

SOURCE=.\snd_sdl\sdl_sound.c
# End Source File
# Begin Source File

SOURCE=.\scommon\swap.c
# End Source File
# Begin Source File

SOURCE=.\os_win32\win32_forcefeedback.c
# End Source File
# End Group
# Begin Group "Header Files"

# PROP Default_Filter "h;hpp;hxx;hm;inl"
# Begin Source File

SOURCE=.\game\game.h
# End Source File
# Begin Source File

SOURCE=.\scommon\keys.h
# End Source File
# Begin Source File

SOURCE=.\scommon\libtarga.h
# End Source File
# Begin Source File

SOURCE=.\scommon\milkshape.h
# End Source File
# Begin Source File

SOURCE=.\ren_gl\ren_gl.h
# End Source File
# Begin Source File

SOURCE=.\scommon\s_common.h
# End Source File
# Begin Source File

SOURCE=.\scommon\s_net.h
# End Source File
# Begin Source File

SOURCE=.\os_sdl\s_sdl.h
# End Source File
# Begin Source File

SOURCE=.\scommon\s_shared.h
# End Source File
# End Group
# Begin Group "Resource Files"

# PROP Default_Filter "ico;cur;bmp;dlg;rc2;rct;bin;rgs;gif;jpg;jpeg;jpe"
# End Group
# End Target
# End Project
