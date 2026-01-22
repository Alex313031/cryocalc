## TODO

 - Port to MSVC make sure MSVC/Ninja/Make flags are all equivalent
 - Add content to .chm file.
 - Make better main icon. Extract higher quality flag.ico from win 2000
 - Add translations in separate imported .RC file for strings.h
 - Check onload/offload dllmain with __cdecl instead of WINAPI.
 - Use CreateFont and SetFont with DS_FONT for Arial instead of shell dlg font.
 - Maybe use CreateThread with ThreadProc for CPU Burner threads.
 - Fix osinfo background color.
 - Add enable logging menu item to attach/detach console.
 - Write full scale Chromium style streaming logger.
 - Convert to std::vector and C++17 types where appropriate.
 - Check for memory leaks/dangling pointers where FreeMemory or LocalFree or delete should be used
 - Reduce flickering by not redrawing some backgrounds and using DeferWindowPos
 - Add hover tooltips for static items
 - Clang-format source
