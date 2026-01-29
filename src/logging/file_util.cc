#include "file_util.h"

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

namespace logging {
HANDLE g_log_file = INVALID_HANDLE_VALUE;
}

bool logging::OpenFileForWriting(std::wstring logfile_path) {
  if (logfile_path.length() >= MAX_PATH) {
    return false;
  }

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
    if (err == ERROR_FILE_EXISTS) {
      // File exists, open it for appending
      g_log_file = CreateFileW(
          logfile_path.c_str(),
          GENERIC_READ | GENERIC_WRITE,
          FILE_SHARE_READ,
          nullptr,
          OPEN_EXISTING,
          FILE_ATTRIBUTE_ARCHIVE | FILE_FLAG_WRITE_THROUGH,
          nullptr);

      if (g_log_file == INVALID_HANDLE_VALUE) {
        std::wcerr << L"[ERROR] Failed to open existing file: " << GetLastError() << std::endl;
        return false;
      } else {
        std::wcout << L"[INFO] Opened existing file " << logfile_path << std::endl;
      }

      // Move to end of file for append mode
      if (SetFilePointer(g_log_file, 0, nullptr, FILE_END) == INVALID_SET_FILE_POINTER &&
          GetLastError() != NO_ERROR) {
        std::wcerr << L"[ERROR] Failed to seek to end of file: " << GetLastError() << std::endl;
        CloseFileHandle();
        return false;
      }
    } else {
      std::wcerr << L"[ERROR] Failed to open file for writing. Error = " << err << std::endl;
      CloseFileHandle();
      return false;
    }
  } else {
    std::wcout << L"[INFO] Creating new log file: " << logfile_path << std::endl;
  }

  return true;
}

bool logging::CloseFileHandle() {
  bool closed = false;
  HANDLE this_handle = g_log_file;
  if (g_log_file != INVALID_HANDLE_VALUE) {
    FlushFileBuffers(g_log_file);
    closed = CloseHandle(g_log_file);
    g_log_file = INVALID_HANDLE_VALUE;
  }
  if (closed) {
    std::wcerr << L"[DEBUG] Closed file handle " << reinterpret_cast<long long>(this_handle) << std::endl;
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

  return result && (bytes_written == utf8_str.length());
}

bool logging::ClearFileContents() {
  if (g_log_file == INVALID_HANDLE_VALUE || !logging_initialized) {
    return false;
  }

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
