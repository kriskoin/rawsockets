# Microsoft Developer Studio Project File - Name="socketClient" - Package Owner=<4>
# Microsoft Developer Studio Generated Build File, Format Version 6.00
# ** DO NOT EDIT **

# TARGTYPE "Win32 (x86) Static Library" 0x0104

CFG=socketClient - Win32 Debug
!MESSAGE This is not a valid makefile. To build this project using NMAKE,
!MESSAGE use the Export Makefile command and run
!MESSAGE 
!MESSAGE NMAKE /f "socketClient.mak".
!MESSAGE 
!MESSAGE You can specify a configuration when running NMAKE
!MESSAGE by defining the macro CFG on the command line. For example:
!MESSAGE 
!MESSAGE NMAKE /f "socketClient.mak" CFG="socketClient - Win32 Debug"
!MESSAGE 
!MESSAGE Possible choices for configuration are:
!MESSAGE 
!MESSAGE "socketClient - Win32 Release" (based on "Win32 (x86) Static Library")
!MESSAGE "socketClient - Win32 Debug" (based on "Win32 (x86) Static Library")
!MESSAGE 

# Begin Project
# PROP AllowPerConfigDependencies 0
# PROP Scc_ProjName ""
# PROP Scc_LocalPath ""
CPP=cl.exe
RSC=rc.exe

!IF  "$(CFG)" == "socketClient - Win32 Release"

# PROP BASE Use_MFC 0
# PROP BASE Use_Debug_Libraries 0
# PROP BASE Output_Dir "Release"
# PROP BASE Intermediate_Dir "Release"
# PROP BASE Target_Dir ""
# PROP Use_MFC 0
# PROP Use_Debug_Libraries 0
# PROP Output_Dir "d:\output\clientLib"
# PROP Intermediate_Dir "d:\output\clientLib"
# PROP Target_Dir ""
# ADD BASE CPP /nologo /W3 /GX /O2 /D "WIN32" /D "NDEBUG" /D "_MBCS" /D "_LIB" /YX /FD /c
# ADD CPP /nologo /MT /W3 /GX /O2 /D "NDEBUG" /D "WIN32" /D "_MBCS" /D "_LIB" /D "MSVCC" /YX /FD /c
# ADD BASE RSC /l 0x409 /d "NDEBUG"
# ADD RSC /l 0x409 /d "NDEBUG"
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LIB32=link.exe -lib
# ADD BASE LIB32 /nologo
# ADD LIB32 /nologo

!ELSEIF  "$(CFG)" == "socketClient - Win32 Debug"

# PROP BASE Use_MFC 0
# PROP BASE Use_Debug_Libraries 1
# PROP BASE Output_Dir "Debug"
# PROP BASE Intermediate_Dir "Debug"
# PROP BASE Target_Dir ""
# PROP Use_MFC 0
# PROP Use_Debug_Libraries 1
# PROP Output_Dir "d:\output\clientLib"
# PROP Intermediate_Dir "d:\output\clientLib"
# PROP Target_Dir ""
# ADD BASE CPP /nologo /W3 /Gm /GX /ZI /Od /D "WIN32" /D "_DEBUG" /D "_MBCS" /D "_LIB" /YX /FD /GZ /c
# ADD CPP /nologo /MT /W3 /Gm /GX /ZI /Od /D "_DEBUG" /D "WIN32" /D "_MBCS" /D "_LIB" /D "MSVCC" /YX /FD /GZ /c
# ADD BASE RSC /l 0x409 /d "_DEBUG"
# ADD RSC /l 0x409 /d "_DEBUG"
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LIB32=link.exe -lib
# ADD BASE LIB32 /nologo
# ADD LIB32 /nologo

!ENDIF 

# Begin Target

# Name "socketClient - Win32 Release"
# Name "socketClient - Win32 Debug"
# Begin Group "Utils"

# PROP Default_Filter ""
# Begin Source File

SOURCE=..\..\..\..\utils\log.cpp
# End Source File
# Begin Source File

SOURCE=..\..\..\..\utils\log.h
# End Source File
# Begin Source File

SOURCE=..\..\..\..\utils\typesDef.h
# End Source File
# Begin Source File

SOURCE=..\..\..\..\utils\utilsFunctions.cpp
# End Source File
# Begin Source File

SOURCE=..\..\..\..\utils\utilsFunctions.h
# End Source File
# End Group
# Begin Group "Locks"

# PROP Default_Filter ""
# Begin Source File

SOURCE=..\..\..\..\locks\criticalSectionsFunctions.cpp
# End Source File
# Begin Source File

SOURCE=..\..\..\..\locks\criticalSectionsFunctions.h
# End Source File
# End Group
# Begin Group "Sockets"

# PROP Default_Filter ""
# Begin Group "Client"

# PROP Default_Filter ""
# Begin Source File

SOURCE=..\..\..\..\sockets\client\socketClient.cpp
# End Source File
# Begin Source File

SOURCE=..\..\..\..\sockets\client\socketClient.h
# End Source File
# End Group
# Begin Group "Packet"

# PROP Default_Filter ""
# Begin Source File

SOURCE=..\..\..\..\sockets\packet\cpacket.cpp
# End Source File
# Begin Source File

SOURCE=..\..\..\..\sockets\packet\cpacket.h
# End Source File
# End Group
# Begin Group "Socket"

# PROP Default_Filter ""
# Begin Source File

SOURCE=..\..\..\..\sockets\socket\csocket.cpp
# End Source File
# Begin Source File

SOURCE=..\..\..\..\sockets\socket\csocket.h
# End Source File
# End Group
# Begin Source File

SOURCE=..\..\..\..\sockets\messages.h
# End Source File
# End Group
# Begin Group "Definitions"

# PROP Default_Filter ""
# Begin Source File

SOURCE=..\..\..\..\definitions\errorsDefinitions.h
# End Source File
# End Group
# End Target
# End Project
