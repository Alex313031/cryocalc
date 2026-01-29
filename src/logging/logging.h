#ifndef MINI_LOGGER_LOGGING_H_
#define MINI_LOGGER_LOGGING_H_

#include "logging_base.h"
#include "check.h"
#include "file_util.h"

enum LogLevel {
  LOG_INFO = 0,
  LOG_WARN = 1,
  LOG_ERROR = 2,
  LOG_DEBUG = 3,
  LOG_FATAL = 4,
  MAX_LOGLEVEL = 5
};

// Toggle to test LOG(FATAL) which will crash the app
static constexpr bool test_fatal = false; 

namespace logging {

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

 protected:
   bool IsDCheck(); // Whether to use DLOG

 private:
  LogLevel level_;
  std::wostringstream stream_;
};

extern volatile bool dcheck_log_;

// Initialize logging for this program
bool InitLogging(HINSTANCE hInstance);

// Set whether to use DLOG
void SetIsDCheck(bool set_is_dcheck);

// Test that logging works as expected.
void TestLogging();

void NotReachedImpl();
}

#define LOG(level) logging::LogMessage(LOG_##level)
#define DLOG() logging::LogMessage(LOG_DEBUG)
#define NOTREACHED() logging::NotReachedImpl()

#endif // MINI_LOGGER_LOGGING_H_
