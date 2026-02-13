#include "console.h"

#include <io.h>

#include "check.h"

namespace logging {
 // For tracking console attach state.
 volatile bool console_attached = false;
 static ATTACH_CONSOLE_ pAttachConsole = nullptr;
}

bool logging::GetIsConsoleAttached() {
  return console_attached;
}

bool logging::AttachConsoleImpl() {
  if (GetIsConsoleAttached()) {
    MessageBoxW(nullptr, L"Console Already Attached!", L"Console Error", MB_OK | MB_ICONERROR);
    return false;
  }
  // Allow and allocate conhost for cmd.exe logging window
  constexpr bool open_cmd_if_none = true;
  const bool attached_console = RouteStdioToConsole(open_cmd_if_none);
  if (!attached_console) {
    MessageBoxW(nullptr, L"Failed to attach console!", L"Console Attach Error", MB_OK | MB_ICONERROR);
  }
  console_attached = attached_console;
  return attached_console;
}

bool logging::DetachConsoleImpl() {
  if (!GetIsConsoleAttached()) {
    MessageBoxW(nullptr, L"Console Already Detached!", L"Console Detach Warning", MB_OK | MB_ICONWARNING);
    return true;
  }
  if (FreeConsole()) {
    console_attached = false;
    return true;
  } else {
    const std::wstring msg = L"Failed to detach console! Error = " + std::to_wstring(GetLastError());
    MessageBoxW(nullptr, msg.c_str(), L"Console Detach Failure", MB_OK | MB_ICONERROR);
    return false;
  }
}

bool logging::RouteStdioToConsole(bool create_console_if_not_found) {
  if (console_attached) {
    std::wcerr << __func__ << L" console_attached = true";
    return true;
  }
  // We don't use GetStdHandle() to check stdout/stderr here because
  // it can return dangling IDs of handles that were never inherited
  // by this process.  These IDs could have been reused by the time
  // this function is called.  The CRT checks the validity of
  // stdout/stderr on startup (before the handle IDs can be reused).
  // _fileno(stdout) will return -2 (_NO_CONSOLE_FILENO) if stdout was
  // invalid.
  if (_fileno(stdout) >= 0 || _fileno(stderr) >= 0) {
    // _fileno was broken for SUBSYSTEM:WINDOWS from VS2010 to VS2012/2013. See http://crbug.com/358267.
    // Confirm that the underlying HANDLE is valid before aborting.
    intptr_t stdout_handle = _get_osfhandle(_fileno(stdout));
    intptr_t stderr_handle = _get_osfhandle(_fileno(stderr));
    if (stdout_handle >= 0 || stderr_handle >= 0) {
      // stdout or stderr already point to a valid stream. Maybe abort?
    }
  }

  pAttachConsole =
      reinterpret_cast<ATTACH_CONSOLE_>(GetProcAddress(GetModuleHandleW(L"kernel32.dll"), "AttachConsole"));
  if (!pAttachConsole) {
    // Didn't get AttachConsole, probably running on Windows 2000, in which case just AllocConsole.
    if (!AllocConsole()) {
      MessageBoxW(nullptr, L"AllocConsole failed!", L"AllocConsole Error", MB_OK | MB_ICONERROR);
      NOTREACHED();
      return false;
    }
  } else {
    if (!pAttachConsole(ATTACH_PARENT_PROCESS)) {
      unsigned int result = GetLastError();
      // Was probably already attached.
      if (result == ERROR_ACCESS_DENIED) {
        MessageBoxW(nullptr, L"ERROR_ACCESS_DENIED", L"AttachConsole_t Error", MB_OK | MB_ICONERROR);
        return false;
      }
      if (create_console_if_not_found) {
        // Make a new console if attaching to parent fails with any other error.
        // It should be ERROR_INVALID_HANDLE at this point, which means the
        // browser was likely not started from a console.
        if (!AllocConsole()) {
          MessageBoxW(nullptr, L"AllocConsole failed!", L"AllocConsole Error", MB_OK | MB_ICONERROR);
          NOTREACHED();
          return false;
        }
      } else {
        MessageBoxW(nullptr, L"Not creating console", L"RouteStdioToConsole Warning", MB_OK | MB_ICONWARNING);
        return false;
      }
    }
  }

  // Arbitrary byte count to use when buffering output lines.  More
  // means potential waste, less means more risk of interleaved
  // log-lines in output.
  enum { kOutputBufferSize = 32 * 1024 };

  if (freopen("CONOUT$", "w", stdout)) {
    setvbuf(stdout, nullptr, _IOLBF, kOutputBufferSize);
    // Overwrite FD 1 for the benefit of any code that uses this FD
    // directly.  This is safe because the CRT allocates FDs 0, 1 and
    // 2 at startup even if they don't have valid underlying Windows
    // handles.  This means we won't be overwriting an FD created by
    // _open() after startup.
    _dup2(_fileno(stdout), 1);
  } else {
    MessageBoxW(nullptr, L"freopen stdout failed!", L"freopen Error", MB_OK | MB_ICONERROR);
  }
  if (freopen("CONOUT$", "w", stderr)) {
    setvbuf(stderr, nullptr, _IOLBF, kOutputBufferSize);
    _dup2(_fileno(stderr), 2);
  } else {
    MessageBoxW(nullptr, L"freopen stderr failed!", L"freopen Error", MB_OK | MB_ICONERROR);
  }

  // Fix all cout, wcout, cin, wcin, cerr, wcerr, clog and wclog together.
  std::ios::sync_with_stdio();
  return true;
}
