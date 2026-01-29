#ifndef MINI_LOGGER_FILE_UTIL_H_
#define MINI_LOGGER_FILE_UTIL_H_

#include "logging_base.h"

namespace logging {

extern HANDLE g_log_file;

// Where to output logging to
enum LogDest {
  LOG_NONE = 0,
  LOG_TO_FILE = 1,
  LOG_TO_STDERR = 2,
  LOG_TO_ALL = 3,
  MAX_LOG_DEST = 4
};

// Convert narrow string to wide string (ASCII only, suitable for __func__, __DATE__, etc.)
inline std::wstring ToWide(const char* s) {
  std::wstring result;
  while (*s) {
    result += static_cast<wchar_t>(*s++);
  }
  return result;
}

// Retrieves the current dir of the .exe calling this process
const std::wstring GetCurrentRelDir();

// Opens a file or creates one if it doesn't exist, and opens it for writing without a process lock
bool OpenFileForWriting(std::wstring logfile_path);

// Closes file handles safely, and sets g_log_file back to INVALID_HANDLE_VALUE
bool CloseFileHandle();

// Appends a line of text to the end of a file.
bool AppendTextToFile(const std::wstring log_line);

// Clears the logfile.
bool ClearFileContents();

}

#endif // MINI_LOGGER_FILE_UTIL_H_
