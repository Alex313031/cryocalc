; CryoCalc Inno Setup Script File
; Only for Inno Setup 5.x to support Windows 2000/XP
; Tested with ISS 5.6.1 (Installer can be found in assets/tools directory)

#define AppVer "0.1.0"
#define AppName "CryoCalc"
#define ExeName "cryocalc"

[Setup]
MinVersion=5.0
AppName={#AppName}
AppVerName={#AppName} Version {#AppVer}
OutputBaseFilename={#ExeName}_{#AppVer}_setup
DefaultDirName={pf}\{#AppName}
DefaultGroupName={#AppName}
UninstallDisplayIcon={app}\{#ExeName}.exe
Compression=lzma
SolidCompression=yes

[Files]
Source: "release\{#ExeName}.exe"; DestDir: "{app}"
Source: "release\osinfo.dll"; DestDir: "{app}"
Source: "release\cryocalc.ini"; DestDir: "{app}"
Source: "assets\chm\cryocalc.chm"; DestDir: "{app}"
Source: "assets\Readme.txt"; DestDir: "{app}"; Flags: isreadme

[Icons]
Name: "{group}\{#AppName}"; Filename: "{app}\{#ExeName}.exe"
