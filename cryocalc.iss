; CryoCalc Inno Setup Script File
; Only for Inno Setup 5.x to support Windows 2000/XP
; Tested with ISS 5.6.1 (Installer can be found in assets/tools directory)

#define AppVer "0.1.0"
#define AppName "CryoCalc"
#define ExeName "cryocalc"
#define Developer "Alex313031"
#define CopyRightYear "© 2025-2026"
#define GitURL "https://github.com/Alex313031/cryocalc"

[Setup]
MinVersion=5.0
AppName={#AppName}
AppVersion={#AppVer}
AppVerName={#AppName} Version {#AppVer}
OutputBaseFilename={#ExeName}_{#AppVer}_setup
UninstallDisplayName=Uninstall {#AppName} v.{#AppVer}
DefaultDirName={pf}\{#AppName}
DefaultGroupName={#AppName}
UninstallDisplayIcon={app}\{#ExeName}.exe
Compression=lzma
SolidCompression=yes
VersionInfoVersion={#AppVer}
AppPublisher={#Developer}
AppPublisherURL={#GitURL}
AppSupportURL={#GitURL}/#readme
AppUpdatesURL={#GitURL}/releases
VersionInfoCompany={#Developer}
VersionInfoCopyright={#CopyRightYear} {#Developer}
VersionInfoProductName={#AppName}

[Files]
Source: "release\{#ExeName}.exe"; DestDir: "{app}"
Source: "release\osinfo.dll"; DestDir: "{app}"
Source: "release\cryocalc.ini"; DestDir: "{app}"
Source: "assets\chm\cryocalc.chm"; DestDir: "{app}"
Source: "assets\Readme.txt"; DestDir: "{app}"; Flags: isreadme

[Icons]
Name: "{group}\{#AppName}"; Filename: "{app}\{#ExeName}.exe"
