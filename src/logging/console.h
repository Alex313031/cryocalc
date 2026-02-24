#ifndef MINI_LOGGER_CONSOLE_H_
#define MINI_LOGGER_CONSOLE_H_

#include "logging_base.h"

typedef BOOL(WINAPI* ATTACH_CONSOLE_)(DWORD dwProcessId);

namespace logging {

  extern volatile bool console_attached;

  // Gets if a console is already attached for this process.
  bool GetIsConsoleAttached();

  // Attaches console to window, only one allowed per process.
  bool AttachConsoleImpl();

  // Detaches console to allow attaching a new one.
  bool DetachConsoleImpl();

  // Opens a console if app wasn't launched from command line, and syncs all logging output to it
  bool RouteStdioToConsole(bool create_console_if_not_found);

} // namespace logging

#endif // MINI_LOGGER_CONSOLE_H_
