#define AppVer "0.3.0"
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
Source: "assets\Readme.txt"; DestDir: "{app}"; Flags: isreadme

[Icons]
Name: "{group}\{#AppName}"; Filename: "{app}\{#ExeName}.exe"
