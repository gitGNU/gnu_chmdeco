; chmdeco -- extract files from ITS/CHM files and decompile CHM files
; Copyright 2003-2007 Paul Wise

; This file is part of chmdeco; you can redistribute it and/or modify
; it under the terms of the GNU General Public License as published by
; the Free Software Foundation; either version 2 of the License, or
; any later version.

; This program is distributed in the hope that it will be useful,
; but WITHOUT ANY WARRANTY; without even the implied warranty of
; MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the
; GNU General Public License for more details.

; You should have received a copy of the GNU General Public License
; along with this program; if not, write to the Free Software Foundation,
; Inc., 59 Temple Place, Suite 330, Boston, MA, 02111-1307, USA or visit:
; http://www.gnu.org

; This is a script to create a NullSoft Install System installer
; for users of Microsoft Windows platforms

; Compression
SetCompress auto
SetCompressor /SOLID lzma

; Compress the installer exe header with UPX
!ifdef UPX
!if "${UPX}" != ""
  !tempfile PACKHDRTEMP
  !packhdr "${PACKHDRTEMP}" '"${UPX}" --best --compress-icons=1 "${PACKHDRTEMP}"'
!endif
!endif

; Package & version come from command line.
; This prevents the need for another .in file.
!define PSV '${PACKAGE} ${VERSION}'
!define PDV '${PACKAGE}-${VERSION}'
!define PDVE '${PDV}.exe'
!define EXE '${PACKAGE}.exe'
!define SMDIR '$SMPROGRAMS\${PSV}'

Name '${PSV}'
Caption 'Compiled HTML Help (CHM) decompiler'
Icon 'chmdeco.ico'
UninstallIcon 'chmdeco.ico'

; Some options
CRCCheck on
ShowInstDetails show
ShowUninstDetails show
XPStyle on
WindowIcon on
CheckBitmap checks.bmp

; The file to write
OutFile '../${PDVE}'

; Licence
LicenseText '${PSV} is licenced under the GNU General Public Licence.$\nNote there are no terms of use, only copying, modification and distribution.'
LicenseData 'COPYING.txt'

; The default installation directory is handled by the .onInit function

; The text to prompt the user to enter a directory
ComponentText 'Which parts of ${PSV} would you like to install on your computer?'

; The text to prompt the user to enter a directory
DirText 'Where would you like to install ${PSV} on your computer?'

; The stuff to install
Section 'Executables (required)'
  SectionIn RO
  SetOutPath $INSTDIR
  File /oname=${EXE} chmdeco.exe
  File istorage.exe
  File chmdeco.ico
  StrCmp $9 "user" user
    WriteRegStr HKLM 'SOFTWARE\${PSV}' 'Install Dir' '$INSTDIR'
    WriteRegStr HKLM 'Software\Microsoft\Windows\CurrentVersion\Uninstall\${PSV}' 'DisplayName' '${PSV}'
    WriteRegStr HKLM 'Software\Microsoft\Windows\CurrentVersion\Uninstall\${PSV}' 'DisplayIcon' '"$INSTDIR\chmdeco.ico"'
    WriteRegStr HKLM 'Software\Microsoft\Windows\CurrentVersion\Uninstall\${PSV}' 'UninstallString' '"$INSTDIR\uninstall.exe"'
    Goto done
  user:
    WriteRegStr HKCU 'SOFTWARE\${PSV}' 'Install Dir' '$INSTDIR'
    WriteRegStr HKCU 'Software\Microsoft\Windows\CurrentVersion\Uninstall\${PSV}' 'DisplayName' '${PSV}'
    WriteRegStr HKCU 'Software\Microsoft\Windows\CurrentVersion\Uninstall\${PSV}' 'DisplayIcon' '"$INSTDIR\chmdeco.ico"'
    WriteRegStr HKCU 'Software\Microsoft\Windows\CurrentVersion\Uninstall\${PSV}' 'UninstallString' '"$INSTDIR\uninstall.exe"'
  done:
  WriteUninstaller 'uninstall.exe'
SectionEnd

; Docs
Section 'Documentation (required)'
  SectionIn RO
  SetOutPath $INSTDIR
  File ${DOCS}
SectionEnd

; Optional Shortcuts
Section 'Start Menu Shortcuts'
  CreateDirectory '${SMDIR}'
  CreateShortCut '${SMDIR}\Uninstall.lnk' '$INSTDIR\uninstall.exe' '' '$INSTDIR\uninstall.exe' 0
  CreateShortCut '${SMDIR}\README.lnk' '$INSTDIR\README.txt' '' '$INSTDIR\chmdeco.ico' 0
  CreateShortCut '${SMDIR}\NEWS.lnk' '$INSTDIR\NEWS.txt' '' '$INSTDIR\chmdeco.ico' 0
  CreateShortCut '${SMDIR}\ChangeLog.lnk' '$INSTDIR\ChangeLog.txt' '' '$INSTDIR\chmdeco.ico' 0
SectionEnd

Section 'SendTo Menu Shortcuts'
  ; FIXME: At the time of writing NSIS didn't yet support $SENDTO
  ; FIXME: Note that there is no Common SendTo folder
  ReadRegStr $R1 HKCU 'Software\Microsoft\Windows\CurrentVersion\Explorer\Shell Folders' 'SendTo'
  CreateShortCut '$R1\Decompile with ${PSV}.lnk' '$INSTDIR\${EXE}' '--' '$INSTDIR\chmdeco.ico' 0
  CreateShortCut '$R1\Decompile with ${PSV} (FTS to HTML).lnk' '$INSTDIR\${EXE}' '-f --' '$INSTDIR\chmdeco.ico' 0
  CreateShortCut '$R1\Unpack with istorage.lnk' '$INSTDIR\istorage.exe' '' '$INSTDIR\chmdeco.ico' 0
SectionEnd

Section 'Context Menu Entries (on Folders)'
  ; FIXME: Use a Context Menu Handler to do this?

  StrCmp $9 "user" user
    WriteRegStr HKCR 'Folder\shell\Decompile with ${PSV}\command' "" '"$INSTDIR\${EXE}" -- "%1"'
    WriteRegStr HKCR 'Folder\shell\Decompile with ${PSV} (FTS to HTML)\command' "" '"$INSTDIR\${EXE}" -f -- "%1"'
    Goto done
  user:
    WriteRegStr HKCU 'Software\Classes\Folder\shell\Decompile with ${PSV}\command' "" '"$INSTDIR\${EXE}" -- "%1"'
    WriteRegStr HKCU 'Software\Classes\Folder\shell\Decompile with ${PSV} (FTS to HTML)\command' "" '"$INSTDIR\${EXE}" -f -- "%1"'
  done:
SectionEnd

; FIXME: Change these when backends are done
Section 'Context Menu Entries (on CHM/ITSs)'
  ; FIXME: Use a Context Menu Handler to do this?

  StrCmp $9 "user" user
    WriteRegStr HKCR 'chm.file\shell\Unpack with istorage\command' "" '"$INSTDIR\istorage.exe" "%1"'
    WriteRegStr HKCR 'ITS FILE\shell\Unpack with istorage\command' "" '"$INSTDIR\istorage.exe" "%1"'
    WriteRegStr HKCR 'MS-ITS File\shell\Unpack with istorage\command' "" '"$INSTDIR\istorage.exe" "%1"'
    Goto done
  user:
    WriteRegStr HKCU 'Software\Classes\chm.file\shell\Unpack with istorage\command' "" '"$INSTDIR\istorage.exe" "%1"'
    WriteRegStr HKCU 'Software\Classes\ITS FILE\shell\Unpack with istorage\command' "" '"$INSTDIR\istorage.exe" "%1"'
    WriteRegStr HKCU 'Software\Classes\MS-ITSS File\shell\Unpack with istorage\command' "" '"$INSTDIR\istorage.exe" "%1"'
  done:
SectionEnd

; Uninstall stuff
UninstallText 'Would you like to remove ${PSV} from your computer?'
Section 'Uninstall'
  ReadRegStr $R1 HKCU 'Software\Microsoft\Windows\CurrentVersion\Explorer\Shell Folders' 'SendTo'
  Delete '$R1\Decompile with ${PSV}.lnk'
  Delete '$R1\Decompile with ${PSV} (FTS to HTML).lnk'
  Delete '$R1\Unpack with istorage.lnk'
  RMDir /r '${SMDIR}'
  RMDir /r '$INSTDIR'
  ; FIXME: Make this smarter wrt user/admin installs
  DeleteRegKey HKCU 'Software\Microsoft\Windows\CurrentVersion\Uninstall\${PSV}'
  DeleteRegKey HKLM 'Software\Microsoft\Windows\CurrentVersion\Uninstall\${PSV}'
  DeleteRegKey HKCU 'SOFTWARE\${PSV}'
  DeleteRegKey HKLM 'SOFTWARE\${PSV}'
; FIXME: Change these when backends are done and remove when context menu handler is done
  DeleteRegKey HKCR 'Folder\shell\Decompile with ${PSV}'
  DeleteRegKey HKCR 'Folder\shell\Decompile with ${PSV} (FTS to HTML)'
  DeleteRegKey /ifempty HKCR 'Folder\shell'
  DeleteRegKey /ifempty HKCR 'Folder'
  DeleteRegKey HKCR 'chm.file\shell\Unpack with istorage'
  DeleteRegKey /ifempty HKCR 'chm.file\shell'
  DeleteRegKey /ifempty HKCR 'chm.file'
  DeleteRegKey HKCR 'ITS FILE\shell\Unpack with istorage'
  DeleteRegKey /ifempty HKCR 'ITS FILE\shell'
  DeleteRegKey /ifempty HKCR 'ITS FILE'
  DeleteRegKey HKCR 'MS-ITS FILE\shell\Unpack with istorage'
  DeleteRegKey /ifempty HKCR 'MS-ITS FILE\shell'
  DeleteRegKey /ifempty HKCR 'MS-ITS FILE'
  DeleteRegKey HKCU 'Software\Classes\Folder\shell\Decompile with ${PSV}'
  DeleteRegKey HKCU 'Software\Classes\Folder\shell\Decompile with ${PSV} (FTS to HTML)'
  DeleteRegKey /ifempty HKCR 'Software\Classes\Folder\shell'
  DeleteRegKey /ifempty HKCR 'Software\Classes\Folder'
  DeleteRegKey HKCU 'Software\Classes\chm.file\shell\Unpack with istorage'
  DeleteRegKey /ifempty HKCR 'Software\Classes\chm.file\shell'
  DeleteRegKey /ifempty HKCR 'Software\Classes\chm.file'
  DeleteRegKey HKCU 'Software\Classes\ITS FILE\shell\Unpack with istorage'
  DeleteRegKey /ifempty HKCR 'Software\Classes\ITS FILE\shell'
  DeleteRegKey /ifempty HKCR 'Software\Classes\ITS FILE'
  DeleteRegKey HKCU 'Software\Classes\MS-ITSS FILE\shell\Unpack with istorage'
  DeleteRegKey /ifempty HKCR 'Software\Classes\MS-ITSS FILE\shell'
  DeleteRegKey /ifempty HKCR 'Software\Classes\MS-ITSS FILE'
SectionEnd

; Customize the install depending on if this is a user/admin install
Function .onInit
	ClearErrors
	UserInfo::GetName
	; on Win9x everyone is admin
	IfErrors admin9x
	Pop $R0
	UserInfo::GetAccountType
	Pop $R1
	StrCmp $R1 "Admin" adminnt
	StrCmp $R1 "Power" user
	StrCmp $R1 "User" user
	StrCmp $R1 "Guest" user

	user:
		MessageBox MB_OK 'Installing ${PSV} for your use only.$\nTo install for all users, restart the installer as an admin user.$\nRight-click the installer file, press "Run as" and enter the admin password.'
	usernoprompt:
		StrCpy $9 "user"
		SetShellVarContext current
		StrCpy $INSTDIR '$DOCUMENTS\${PSV}'
		Goto done

	adminnt:
		MessageBox MB_YESNO|MB_ICONQUESTION "Do you want to install ${PSV} for all users?$\nNote that Windows does not provide a common SendTo folder,$\nso you have to manually copy any SendTo links to the SendTo folder of each user." IDNO usernoprompt
	admin9x:
		StrCpy $9 "admin"
		SetShellVarContext all
		StrCpy $INSTDIR '$PROGRAMFILES\${PSV}'

	done:
FunctionEnd

; Customize the uninstall depending on if this is a user/admin install
Function un.onInit
	ClearErrors
	UserInfo::GetName
	; on Win9x everyone is admin
	IfErrors admin9x
	Pop $R0
	UserInfo::GetAccountType
	Pop $R1
	StrCmp $R1 "Admin" adminnt
	StrCmp $R1 "Power" user
	StrCmp $R1 "User" user
	StrCmp $R1 "Guest" user

	user:
		SetShellVarContext current
		Goto done

	adminnt:
	admin9x:
		SetShellVarContext all

	done:
FunctionEnd
