#include "file_util.h"

#include "check.h"

namespace logging {
 HANDLE g_log_file = INVALID_HANDLE_VALUE;
 volatile bool file_open = false;
}

const std::wstring logging::GetCurrentRelDir() {
  wchar_t exe_path[MAX_PATH];
  HMODULE this_app = GetModuleHandleW(nullptr);
  if (!this_app) {
    return std::wstring();
  }
  DWORD got_path = GetModuleFileNameW(this_app, exe_path, MAX_PATH);
  if (got_path == 0 || got_path >= MAX_PATH) {
    return std::wstring();
  }

  // Find the last backslash to get the directory
  std::wstring fullPath(exe_path);
  size_t lastSlash = fullPath.find_last_of(L"\\/");
  std::wstring retval;
  if (lastSlash != std::wstring::npos) {
    retval = fullPath.substr(0, lastSlash + 1);  // Include trailing slash
  } else {
    retval = fullPath;
  }
  return retval;
}

bool logging::OpenFileForWriting(std::wstring logfile_path) {
  if (logfile_path.length() >= MAX_PATH) {
    return false;
  }
  CHECK(!file_open);
  const bool is_console_attached = GetIsConsoleAttached();
  // Try to create a new file first
  g_log_file = CreateFileW(
      logfile_path.c_str(),
      GENERIC_READ | GENERIC_WRITE,
      FILE_SHARE_READ,
      nullptr,        // Default security
      CREATE_NEW,     // Fail if file exists
      FILE_ATTRIBUTE_ARCHIVE | FILE_FLAG_WRITE_THROUGH,
      nullptr);

  if (g_log_file == INVALID_HANDLE_VALUE) {
    DWORD err = GetLastError();
    std::wstring msg = L"";
    if (err == ERROR_FILE_EXISTS) {
      // File exists, open it for appending
      g_log_file = CreateFileW(
          logfile_path.c_str(),
          GENERIC_READ | GENERIC_WRITE,
          FILE_SHARE_READ,
          nullptr,
          OPEN_EXISTING,
          FILE_ATTRIBUTE_NORMAL,
          nullptr);

      if (g_log_file == INVALID_HANDLE_VALUE) {
        msg = L"Failed to open existing file. Error = " + std::to_wstring(GetLastError());
        MessageBoxW(nullptr, msg.c_str(), L"Open File Error", MB_OK | MB_ICONERROR);
        file_open = false;
        return false;
      } else {
        file_open = true;
      }

      // Move to end of file for append mode
      if (SetFilePointer(g_log_file, 0, nullptr, FILE_END) == INVALID_SET_FILE_POINTER &&
          GetLastError() != NO_ERROR) {
        msg = L"Failed to seek to end of file. Error = " + std::to_wstring(GetLastError());
        MessageBoxW(nullptr, msg.c_str(), L"Open File Error", MB_OK | MB_ICONERROR);
        CloseFileHandle();
        return false;
      }
    } else {
      msg = L"Failed to open file for writing. Error = " + std::to_wstring(GetLastError());
      MessageBoxW(nullptr, msg.c_str(), L"Open File Error", MB_OK | MB_ICONERROR);
      CloseFileHandle();
      return false;
    }
  } else {
    if (is_console_attached) {
      std::wcout << L"Note: Creating new log file: " << logfile_path << std::endl;
    }
    file_open = true;
  }

  return true;
}

bool logging::CloseFileHandle() {
  bool closed = false;
  CHECK(file_open);
  HANDLE kFileHandle = g_log_file;
  const std::wstring this_handle = std::to_wstring(reinterpret_cast<long long>(kFileHandle));
  if (g_log_file != INVALID_HANDLE_VALUE) {
    FlushFileBuffers(g_log_file);
    closed = CloseHandle(g_log_file);
  }
  if (closed) {
    g_log_file = INVALID_HANDLE_VALUE;
    file_open = false;
    std::wcerr << L"[DEBUG] Closed file handle " << this_handle.c_str() << std::endl;
  } else {
    const std::wstring msg = L"Failed to close file handle " + this_handle;
    MessageBoxW(nullptr, msg.c_str(), L"CloseFileHandle Error", MB_OK | MB_ICONERROR);
  }
  return closed;
}

bool logging::AppendTextToFile(const std::wstring log_line) {
  if (g_log_file == INVALID_HANDLE_VALUE || !logging_initialized) {
    return false;
  }

  // Append newline to the log line
  std::wstring line_with_newline = log_line + L"\r\n";

  // Convert wide string to UTF-8 for file output
  int utf8_len = WideCharToMultiByte(CP_UTF8, 0, line_with_newline.c_str(),
                                      static_cast<int>(line_with_newline.length()),
                                      nullptr, 0, nullptr, nullptr);
  if (utf8_len == 0) {
    return false;
  }

  std::string utf8_str(utf8_len, '\0');
  WideCharToMultiByte(CP_UTF8, 0, line_with_newline.c_str(),
                      static_cast<int>(line_with_newline.length()),
                      &utf8_str[0], utf8_len, nullptr, nullptr);

  DWORD bytes_written = 0;
  BOOL result = WriteFile(g_log_file, utf8_str.c_str(),
                          static_cast<DWORD>(utf8_str.length()),
                          &bytes_written, nullptr);

  if (result && (bytes_written == utf8_str.length())) {
    return FlushFileBuffers(g_log_file);
  }
  return false;
}

bool logging::ClearFileContents() {
  if (g_log_file == INVALID_HANDLE_VALUE || !logging_initialized) {
    return false;
  }

  // Flush any pending writes before truncating
  FlushFileBuffers(g_log_file);

  // Move to beginning of file
  if (SetFilePointer(g_log_file, 0, nullptr, FILE_BEGIN) == INVALID_SET_FILE_POINTER &&
      GetLastError() != NO_ERROR) {
    return false;
  }

  // Truncate file at current position (beginning)
  if (!SetEndOfFile(g_log_file)) {
    return false;
  }

  return true;
}

bool logging::IsFileOpen() {
  if (g_log_file != INVALID_HANDLE_VALUE && file_open) {
    return true;
  }
  return false;
}
