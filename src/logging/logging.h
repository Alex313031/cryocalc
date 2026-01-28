#ifndef CRYOCALC_LOGGING_H_
#define CRYOCALC_LOGGING_H_

#include "../framework.h"
#include "../globals.h"

enum LogLevel {
  LOG_INFO = 0,
  LOG_WARN = 1,
  LOG_ERROR = 2,
  LOG_DEBUG = 3,
  LOG_FATAL = 4,
  MAX_LOGLEVEL = 5
};

class LogMessage {
 public:
  explicit LogMessage(LogLevel level);
  ~LogMessage();

  LogMessage(const LogMessage&) = delete;
  LogMessage& operator=(const LogMessage&) = delete;

  // Narrow string/char overloads - convert to wide
  LogMessage& operator<<(char value);
  LogMessage& operator<<(const char* value);

  // Wide string/char overloads - stream directly
  LogMessage& operator<<(wchar_t value);
  LogMessage& operator<<(const wchar_t* value);
  LogMessage& operator<<(const std::string& value);
  LogMessage& operator<<(const std::wstring& value);

  // Numeric type overloads
  LogMessage& operator<<(int value);
  LogMessage& operator<<(unsigned int value);
  LogMessage& operator<<(long value);
  LogMessage& operator<<(unsigned long value);
  LogMessage& operator<<(long long value);
  LogMessage& operator<<(unsigned long long value);
  LogMessage& operator<<(float value);
  LogMessage& operator<<(double value);
  LogMessage& operator<<(long double value);

  // Other overloads
  LogMessage& operator<<(HWND value);

  // Generic template for streams and other types (manipulators, etc.)
  template <typename T>
  LogMessage& operator<<(const T& value) {
    stream_ << value;
    return *this;
  }

 private:
  LogLevel level_;
  std::wostringstream stream_;
};

#define LOG(level) LogMessage(LOG_##level)
#define DLOG(level) LogMessage(LOG_##level)

// Initialize logging for this program
bool InitLogging(HINSTANCE hInstance);

// Test that logging works as expected.
void TestLogging();

#endif // CRYOCALC_LOGGING_H_
