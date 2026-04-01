; CryoCalc Inno Setup Script File
; Only for Inno Setup 5.x to support Windows 2000/XP
; Tested with ISS 5.6.1 (Installer can be found in assets/tools directory)

#define AppVer "0.1.8"
#define AppName "CryoCalc"
#define ExeName "cryocalc"
#define Developer "Alex313031"
#define CopyRightYear "© 2025-2026"
#define GitURL "https://github.com/Alex313031/cryocalc"

[Setup]
MinVersion=5.0
AppName={#AppName}
AppVersion={#AppVer}
AppVerName={#AppName} v{#AppVer}
OutputBaseFilename={#ExeName}_{#AppVer}_setup
UninstallDisplayName={#AppName} {#AppVer}
DefaultDirName={pf}\{#AppName}
DefaultGroupName={#AppName}
UninstallDisplayIcon={app}\{#ExeName}.exe
DisableWelcomePage=no
AlwaysShowDirOnReadyPage=yes
Compression=lzma
SolidCompression=yes
VersionInfoVersion={#AppVer}
AppPublisher={#Developer}
AppPublisherURL={#GitURL}
AppSupportURL={#GitURL}/#readme
AppUpdatesURL={#GitURL}/releases
VersionInfoCompany={#Developer}
AppCopyright={#CopyRightYear}
VersionInfoCopyright={#CopyRightYear} {#Developer}
VersionInfoProductName={#AppName}
InfoBeforeFile="assets\License.txt"
;InfoAfterFile="assets\Readme.txt"
SetupIconFile="src\res\cryocalc.ico"
WizardImageFile="assets\installer_background.bmp"
;WizardStyle=modern
UseSetupLdr=yes

[Files]
Source: "release\{#ExeName}.exe"; DestDir: "{app}"
Source: "release\osinfo.dll"; DestDir: "{app}"
Source: "assets\cryocalc.ini"; DestDir: "{app}"
Source: "assets\chm\cryocalc.chm"; DestDir: "{app}"
Source: "assets\License.txt"; DestDir: "{app}"
Source: "assets\Readme.txt"; DestDir: "{app}";

[Icons]
Name: "{group}\{#AppName}"; Filename: "{app}\{#ExeName}.exe"

[Run]
Filename: "{app}\Readme.txt"; Description: "View Readme.txt"; Flags: postinstall nowait skipifsilent shellexec unchecked
Filename: "{app}\{#ExeName}.exe"; Description: "Launch CryoCalc when finished"; Flags: postinstall nowait skipifsilent
