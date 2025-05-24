#pragma once

EXTERN_C_START

_IRQL_requires_max_(PASSIVE_LEVEL)
BOOL APIENTRY MUSA_NAME(GetFileVersionInfoW)(
    _In_ LPCWSTR                      FileName,   /* Filename of version stamped file */
    _Reserved_ DWORD                  Handle,       /* Information from GetFileVersionSize */
    _In_ DWORD                        Length,       /* Length of buffer for info */
    _Out_writes_bytes_(Length) LPVOID Data   /* Buffer to place the data structure */
    );

_IRQL_requires_max_(PASSIVE_LEVEL)
DWORD APIENTRY MUSA_NAME(GetFileVersionInfoSizeW)(
    _In_ LPCWSTR      FileName,  /* Filename of version stamped file */
    _Out_opt_ LPDWORD Handle     /* Information for use by GetFileVersionInfo */
    );

EXTERN_C_END
