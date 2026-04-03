#include "common.h"

#include <subauth.h>

typedef NTSTATUS(NTAPI* RtlAdjustPrivilege_t)(ULONG Privilege,
                                              BOOLEAN Enable,
                                              BOOLEAN CurrentThread,
                                              PBOOLEAN Enabled);

typedef NTSTATUS(NTAPI* NtRaiseHardError_t)(NTSTATUS ErrorStatus,
                                            ULONG NumberOfParameters,
                                            ULONG UnicodeStringParameterMask,
                                            PULONG_PTR Parameters,
                                            ULONG ResponseOption,
                                            PULONG Response);

static RtlAdjustPrivilege_t pfnRtlAdjustPrivilege = nullptr;
static NtRaiseHardError_t pfnNtRaiseHardError = nullptr;

typedef enum _HARDERROR_RESPONSE_OPTION {
  OptionAbortRetryIgnore,
  OptionOk,
  OptionOkCancel,
  OptionRetryCancel,
  OptionYesNo,
  OptionYesNoCancel,
  OptionShutdownSystem
} HARDERROR_RESPONSE_OPTION;

typedef enum _HARDERROR_RESPONSE {
  ResponseReturnToCaller,
  ResponseNotHandled,
  ResponseAbort,
  ResponseCancel,
  ResponseIgnore,
  ResponseNo,
  ResponseOk,
  ResponseRetry,
  ResponseYes
} HARDERROR_RESPONSE;

void RtlInitUnicodeStringCompat(PUNICODE_STRING dest, LPCWSTR source) {
  if (source == nullptr) {
    dest->Length        = 0;
    dest->MaximumLength = 0;
    dest->Buffer        = nullptr;
  } else {
    const USHORT len    = static_cast<USHORT>(wcslen(source) * sizeof(WCHAR));
    dest->Length        = len;
    dest->MaximumLength = len + static_cast<USHORT>(sizeof(WCHAR));
    dest->Buffer        = const_cast<PWSTR>(source);
  }
}

extern "C" ULONG BSOD(const std::wstring& bsod_caption, const std::wstring& bsod_text, bool fatal) {
  const LPCWSTR text = bsod_text.c_str();
  const LPCWSTR caption = bsod_caption.c_str();
  // Just use max ULONG value as invalid: no Windows error codes go that high.
  static const ULONG kInvalid =
      static_cast<ULONG>(std::numeric_limits<unsigned long>::max());
  HMODULE ntdll = LoadLibraryW(L"ntdll.dll");
  if (ntdll == nullptr) {
    LOG(FATAL) << L"ntdll.dll STATUS_DLL_INIT_FAILED";
    return kInvalid;
  }

  pfnRtlAdjustPrivilege =
      reinterpret_cast<RtlAdjustPrivilege_t>(GetProcAddress(ntdll, "RtlAdjustPrivilege"));
  pfnNtRaiseHardError =
      reinterpret_cast<NtRaiseHardError_t>(GetProcAddress(ntdll, "NtRaiseHardError"));

  if (pfnRtlAdjustPrivilege == nullptr) {
    LOG(ERROR) << L"Failed to get RtlAdjustPrivilege function from ntdll.dll.";
    return kInvalid;
  }
  if (pfnNtRaiseHardError == nullptr) {
    LOG(ERROR) << L"Failed to get NtRaiseHardError function from ntdll.dll.";
    return kInvalid;
  }

  // Activate shutdown privilege
  BOOLEAN bEnabled = FALSE;
  NTSTATUS shutdown_priv = pfnRtlAdjustPrivilege(19UL, TRUE, FALSE, &bEnabled);
  if (!NT_SUCCESS(shutdown_priv)) {
    LOG(ERROR) << L"Failed to get Shutdown Privilege.";
    return kInvalid;
  }

  // NtSetDefaultHardErrorPort(COM1)

  // System is going down now!
  NTSTATUS status = static_cast<NTSTATUS>(0xC0000420UL); // STATUS_ASSERTION_FAILURE
  UNICODE_STRING kText, kCaption;
  RtlInitUnicodeStringCompat(&kText, text);
  RtlInitUnicodeStringCompat(&kCaption, caption);
  // Fix: params must be ULONG_PTR[], not PULONG_PTR[]. PULONG_PTR is ULONG_PTR*,
  // so PULONG_PTR[3] is an array of pointers-to-ULONG_PTR — one extra level of
  // indirection. NtRaiseHardError reads Parameters as a flat ULONG_PTR array,
  // casting each entry to PUNICODE_STRING for bits set in UnicodeStringParameterMask.
  // The spurious third entry (0x0420) is also removed: display style is set by
  // ResponseOption, not via a Parameters entry.
  ULONG param_mask  = 3UL; // bits 0+1: both params are UNICODE_STRINGs
  ULONG_PTR params[2];
  params[0] = (ULONG_PTR)&kText;
  params[1] = (ULONG_PTR)&kCaption;
  ULONG num_params = static_cast<ULONG>(ARRAYSIZE(params));
  ULONG type   = fatal ? OptionShutdownSystem : OptionOk;
  ULONG retval = fatal ? ResponseNotHandled : ResponseNotHandled; // ResponseReturnToCaller
  // Undocumented BSOD bugcheck function
  NTSTATUS death = pfnNtRaiseHardError(status, num_params, param_mask, params, type, &retval);
  if (!NT_SUCCESS(death)) {
    LOG(ERROR) << L"NtRaiseHardError failed!";
    return kInvalid;
  }
  if (retval == kInvalid) {
    LOG(FATAL) << L"NtRaiseHardError unexpected result.";
  }
  return retval;
}
