## TODO

 - Port to MSVC make sure MSVC/Ninja/Make flags are all equivalent
 - Add content to .chm file.
 - Make better main icon. Extract higher quality icos from Win 2000
 - Add translations in separate imported .RC file for strings.h
 - Check onload/offload dllmain with __cdecl instead of WINAPI.
 - Use CreateFont and SetFont with DS_FONT for Arial instead of shell dlg font.
 - Maybe use CreateThread with ThreadProc for CPU Burner threads.
 - Write full scale Chromium style streaming logger.
 - Convert to std::vector and C++17 types where appropriate.
 - Reduce flickering by not redrawing some backgrounds and using DeferWindowPos
 - Fix hover tooltips not showing anymore after press, and not showing at all on Win2K.
 - Fix max size for log cleanup.
 - Better colors for controls
 - Clang-format source
 - Add out of bounds checking for absolute zero/plank temp.
