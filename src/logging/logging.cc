#include "logging.h"

#include <limits>

LogMessage::LogMessage(LogLevel level) : level_(level) {}

LogMessage::~LogMessage() {
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
      prefix = L"[DEBUG] ";
      break;
    case LOG_FATAL:
      prefix = L"[FATAL] ";
      break;
    case MAX_LOGLEVEL:
      return;
    default:
      prefix = L"[UNKNOWN] ";
      break;
  }

  if (level_ == LOG_FATAL) {
    std::wcerr << prefix << stream_.str() << std::endl;
#ifdef _DEBUG
    __debugbreak();
#else
    ExitProcess(LOG_FATAL);
#endif
    return;
  }

  if (level_ == LOG_INFO) {
    std::wcout << prefix << stream_.str() << std::endl;
  } else {
    std::wcerr << prefix << stream_.str() << std::endl;
  }
}

LogMessage& LogMessage::operator<<(char value) {
  stream_ << static_cast<wchar_t>(value);
  return *this;
}

LogMessage& LogMessage::operator<<(const char* value) {
  if (value) {
    while (*value) {
      stream_ << static_cast<wchar_t>(*value);
      ++value;
    }
  }
  return *this;
}

LogMessage& LogMessage::operator<<(wchar_t value) {
  stream_ << value;
  return *this;
}

LogMessage& LogMessage::operator<<(const wchar_t* value) {
  if (value) {
    stream_ << value;
  }
  return *this;
}

LogMessage& LogMessage::operator<<(const std::string& value) {
  for (char c : value) {
    stream_ << static_cast<wchar_t>(c);
  }
  return *this;
}

LogMessage& LogMessage::operator<<(const std::wstring& value) {
  stream_ << value;
  return *this;
}

LogMessage& LogMessage::operator<<(int value) {
  stream_ << value;
  return *this;
}

LogMessage& LogMessage::operator<<(unsigned int value) {
  stream_ << value;
  return *this;
}

LogMessage& LogMessage::operator<<(long value) {
  stream_ << value;
  return *this;
}

LogMessage& LogMessage::operator<<(long long value) {
  stream_ << value;
  return *this;
}

LogMessage& LogMessage::operator<<(unsigned long value) {
  stream_ << value;
  return *this;
}

LogMessage& LogMessage::operator<<(unsigned long long value) {
  stream_ << value;
  return *this;
}

LogMessage& LogMessage::operator<<(float value) {
  stream_ << std::setprecision(std::numeric_limits<float>::digits10) << value;
  return *this;
}

LogMessage& LogMessage::operator<<(double value) {
  stream_ << std::setprecision(std::numeric_limits<double>::digits10) << value;
  return *this;
}

LogMessage& LogMessage::operator<<(long double value) {
  stream_ << std::setprecision(std::numeric_limits<long double>::digits10) << value;
  return *this;
}

LogMessage& LogMessage::operator<<(HWND value) {
  stream_ << std::fixed << std::showbase << std::hex
          << reinterpret_cast<unsigned long long>(value) << std::dec << std::noshowbase << std::defaultfloat;
  return *this;
}

bool InitLogging(HINSTANCE hInstance) {
  if (!hInstance) {
    return false;
  }
  return true;
}

void TestLogging() {
  std::cout << "Testing logging with different types " << std::endl;
  LOG(INFO) << "Info1: ostream " << L"Info2 wostream ";
  static constexpr char info3[] = "Info3 char ";
  static constexpr wchar_t info4[] = L"Info4 wchar_t ";
  LOG(INFO) << info3 << info4;
  static const std::string info5 = "Info5 string ";
  static const std::wstring info6 = L"Info6 wstring ";
  LOG(INFO) << info5 << info6;
  std::ostringstream info7;
  std::wostringstream info8;
  info7 << "Info7 ostringstream ";
  info8 << L"Info8 wostringstream ";
  LOG(INFO) << info7.str() << info8.str();

  static constexpr unsigned long long testULL = std::numeric_limits<unsigned long long>::max();
  static constexpr float testFl = 3.141592f;
  static constexpr long double testDb = 3.141592653589793238462643383279L;
  static constexpr DWORD testDword = static_cast<DWORD>(0x0003);
  LOG(WARN) << "Test DWORD: " << testDword;
  LOG(WARN) << "Test float: " << testFl;
  LOG(WARN) << "Test unsigned long long: " <<  testULL;
  LOG(WARN) << "Test long double: " <<  testDb;
  LOG(ERROR) << "Test Error";
}
