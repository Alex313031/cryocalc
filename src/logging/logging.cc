#include "logging.h"

namespace logging {
 volatile bool dcheck_log_ = false;
 bool logging_initialized = false;
}

logging::LogMessage::LogMessage(LogLevel level) : level_(level) {}

logging::LogMessage::~LogMessage() {
  if (!logging_initialized) {
    OutputDebugStringW(L"Logging not initialized!");
    return;
  }

  const wchar_t* prefix;
  if (level_ == MAX_LOGLEVEL) {
    std::wcerr << L"MAX_LOGLEVEL reached!" << std::endl;
  }
  switch (level_) {
    case LOG_INFO:
      prefix = L"[INFO] ";
      break;
    case LOG_WARN:
      prefix = L"[WARN] ";
      break;
    case LOG_ERROR:
      prefix = L"[ERROR] ";
      break;
    case LOG_DEBUG:
      prefix = IsDCheck() ? L"[DCHECK] " : L"[DEBUG] ";
      break;
    case LOG_FATAL:
      prefix = L"[FATAL] ";
      break;
    case MAX_LOGLEVEL:
    default:
      std::wcerr << ToWide(__func__) << L"INVALID LOG LEVEL" << std::endl;
      NOTREACHED();
      return;
  }

  if (level_ == LOG_FATAL) {
    std::wcerr << prefix << stream_.str() << std::endl;
#ifdef _DEBUG
    // Catch for debugger on FATAL
    __debugbreak();
#else
    // Crash more gracefully
    ExitProcess(LOG_FATAL);
#endif
    return;
  } else if (level_ == LOG_INFO) {
    std::wcout << prefix << stream_.str() << std::endl;
  } else {
    // Levels higher than INFO level go to stderr
    std::wcerr << prefix << stream_.str() << std::endl;
  }
  AppendTextToFile(std::wstring(prefix) + stream_.str());
}

logging::LogMessage& logging::LogMessage::operator<<(char value) {
  stream_ << static_cast<wchar_t>(value);
  return *this;
}

logging::LogMessage& logging::LogMessage::operator<<(const char* value) {
  if (value) {
    while (*value) {
      stream_ << static_cast<wchar_t>(*value);
      ++value;
    }
  }
  return *this;
}

logging::LogMessage& logging::LogMessage::operator<<(wchar_t value) {
  stream_ << value;
  return *this;
}

logging::LogMessage& logging::LogMessage::operator<<(const wchar_t* value) {
  if (value) {
    stream_ << value;
  }
  return *this;
}

logging::LogMessage& logging::LogMessage::operator<<(const std::string& value) {
  for (char c : value) {
    stream_ << static_cast<wchar_t>(c);
  }
  return *this;
}

logging::LogMessage& logging::LogMessage::operator<<(const std::wstring& value) {
  stream_ << value;
  return *this;
}

logging::LogMessage& logging::LogMessage::operator<<(int value) {
  stream_ << value;
  return *this;
}

logging::LogMessage& logging::LogMessage::operator<<(unsigned int value) {
  stream_ << value;
  return *this;
}

logging::LogMessage& logging::LogMessage::operator<<(long value) {
  stream_ << value;
  return *this;
}

logging::LogMessage& logging::LogMessage::operator<<(long long value) {
  stream_ << value;
  return *this;
}

logging::LogMessage& logging::LogMessage::operator<<(unsigned long value) {
  stream_ << value;
  return *this;
}

logging::LogMessage& logging::LogMessage::operator<<(unsigned long long value) {
  stream_ << value;
  return *this;
}

logging::LogMessage& logging::LogMessage::operator<<(float value) {
  stream_ << std::setprecision(std::numeric_limits<float>::digits10) << value;
  return *this;
}

logging::LogMessage& logging::LogMessage::operator<<(double value) {
  stream_ << std::setprecision(std::numeric_limits<double>::digits10) << value;
  return *this;
}

logging::LogMessage& logging::LogMessage::operator<<(long double value) {
  stream_ << std::setprecision(std::numeric_limits<long double>::digits10) << value;
  return *this;
}

logging::LogMessage& logging::LogMessage::operator<<(HWND value) {
  stream_ << std::fixed << std::showbase << std::hex
          << reinterpret_cast<unsigned long long>(value) << std::dec << std::noshowbase << std::defaultfloat;
  return *this;
}

// TODO: Add DCHECK/DLOG
bool logging::LogMessage::IsDCheck() {
 return dcheck_log_;
}

void logging::SetIsDCheck(bool set_is_dcheck) {
  dcheck_log_ = set_is_dcheck;
}

bool logging::InitLogging(HINSTANCE hInstance, LogDest log_sink, const std::wstring logfile_name) {
  bool success = false;
  if (!hInstance || log_sink >= MAX_LOG_DEST) {
    logging_initialized = false;
    return false;
  }
  const std::wstring logfile = GetCurrentRelDir() + logfile_name;
  switch (log_sink) {
    case LOG_NONE:
    case MAX_LOG_DEST:
      logging_initialized = false;
      break;
    case LOG_TO_FILE: {
      if (!OpenFileForWriting(logfile)) {
        success = false;
      } else {
        success = true;
      }
    } break;
    case LOG_TO_ALL: {
      if (!OpenFileForWriting(logfile)) {
        success = false;
      } else {
        success = true;
      }
    } break;
    case LOG_TO_STDERR:
      success = true;
      break;
    default:
      NOTREACHED();
      break;
  }
  logging_initialized = success;

  // TODO: Add code to log to file optionally
  return success;
}

bool logging::DeInitLogging(HINSTANCE hInstance) {
  if (!hInstance) {
    return false;
  }
  return CloseFileHandle();
}

// Tests the various operator overloads for basic types.
void logging::TestLogging() {
  std::cout << "[INFO] Testing logging with different types " << std::endl;
  LOG(INFO) << "Info1: ostream " << L"Info2 wostream ";
  static constexpr char info3[] = "Info3 char ";
  static constexpr wchar_t info4[] = L"Info4 wchar_t ";
  static const std::string info5 = "Info5 string ";
  static const std::wstring info6 = L"Info6 wstring ";
  std::ostringstream info7;
  std::wostringstream info8;
  info7 << "Info7 ostringstream ";
  info8 << L"Info8 wostringstream ";
  static constexpr float testFl = 3.141592f;
  static constexpr unsigned long long testULL = std::numeric_limits<unsigned long long>::max();
  static constexpr long double testDb = 3.141592653589793238462643383279L;
  static constexpr DWORD testDword = static_cast<DWORD>(0x0003);
  LOG(INFO) << info3 << info4;
  LOG(INFO) << info5 << info6;
  LOG(INFO) << info7.str() << info8.str();
  LOG(WARN) << "Test DWORD: " << testDword;
  LOG(WARN) << "Test float: " << testFl;
  LOG(DEBUG) << "Test unsigned long long: " <<  testULL;
  LOG(DEBUG) << "Test long double: " <<  testDb;
  LOG(ERROR) << "Test Error";
  LOG(ERROR) << L"Test Error " << GetLastError();
  DLOG() << L"DLOG Test";
  if (test_fatal) {
    LOG(FATAL) << L"Testing wide character FATAL logging";
  }
  AppendTextToFile(L"Hello world hawklogging");
}

void logging::NotReachedImpl() {
  // TODO, add way to get function name from caller from PiCalc
  std::wstring msg = L"NOTREACHED(): " + ToWide(__func__);
  OutputDebugStringW(msg.c_str());
  __debugbreak();
}
