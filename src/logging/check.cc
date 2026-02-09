#include "check.h"

#include "logging.h"

void logging::CheckImpl(const char* condition, bool check_flag) {
  if (check_flag) {
    LOG(FATAL) << "Check failed: " << condition;
  }
}

void logging::NotReachedImpl() {
  // TODO, add way to get function name from caller from PiCalc
  std::wstring msg = L"NOTREACHED(): " + ToWide(__func__);
  OutputDebugStringW(msg.c_str());
  __debugbreak();
}
