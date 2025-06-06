﻿#pragma once

EXTERN_C_START

_IRQL_requires_max_(PASSIVE_LEVEL)
BOOL WINAPI MUSA_NAME(GetProcessTimes)(
    _In_ HANDLE      ProcessHandle,
    _Out_ LPFILETIME CreationTime,
    _Out_ LPFILETIME ExitTime,
    _Out_ LPFILETIME KernelTime,
    _Out_ LPFILETIME UserTime
    );

_IRQL_requires_max_(PASSIVE_LEVEL)
BOOL WINAPI MUSA_NAME(TerminateProcess)(
    _In_ HANDLE ProcessHandle,
    _In_ UINT   ExitCode
    );

_IRQL_requires_max_(PASSIVE_LEVEL)
BOOL WINAPI MUSA_NAME(GetExitCodeProcess)(
    _In_ HANDLE   ProcessHandle,
    _Out_ LPDWORD ExitCode
    );

_IRQL_requires_max_(PASSIVE_LEVEL)
DWORD WINAPI MUSA_NAME(GetProcessVersion)(
    _In_ DWORD ProcessId
    );

_IRQL_requires_max_(PASSIVE_LEVEL)
BOOL WINAPI MUSA_NAME(OpenProcessToken)(
    _In_ HANDLE      ProcessHandle,
    _In_ DWORD       DesiredAccess,
    _Outptr_ PHANDLE TokenHandle
    );

_IRQL_requires_max_(PASSIVE_LEVEL)
BOOL WINAPI MUSA_NAME(SetPriorityClass)(
    _In_ HANDLE ProcessHandle,
    _In_ DWORD  PriorityClass
    );

_IRQL_requires_max_(PASSIVE_LEVEL)
BOOL WINAPI MUSA_NAME(GetPriorityClass)(
    _In_ HANDLE ProcessHandle
    );

_IRQL_requires_max_(PASSIVE_LEVEL)
BOOL WINAPI MUSA_NAME(ProcessIdToSessionId)(
    _In_ DWORD   ProcessId,
    _Out_ DWORD* SessionId
    );

_IRQL_requires_max_(PASSIVE_LEVEL)
DWORD WINAPI MUSA_NAME(GetProcessId)(
    _In_ HANDLE ProcessHandle
    );

_IRQL_requires_max_(PASSIVE_LEVEL)
BOOL WINAPI MUSA_NAME(FlushInstructionCache)(
    _In_ HANDLE                        ProcessHandle,
    _In_reads_bytes_opt_(Size) LPCVOID BaseAddress,
    _In_ SIZE_T                        Size
    );

_IRQL_requires_max_(PASSIVE_LEVEL)
VOID WINAPI MUSA_NAME(FlushProcessWriteBuffers)(VOID);

_IRQL_requires_max_(PASSIVE_LEVEL)
DWORD WINAPI MUSA_NAME(GetProcessIdOfThread)(
    _In_ HANDLE Thread
    );

_IRQL_requires_max_(PASSIVE_LEVEL)
HANDLE WINAPI MUSA_NAME(OpenProcess)(
    _In_ DWORD DesiredAccess,
    _In_ BOOL  InheritHandle,
    _In_ DWORD ProcessId
    );

_IRQL_requires_max_(PASSIVE_LEVEL)
BOOL WINAPI MUSA_NAME(GetProcessHandleCount)(
    _In_ HANDLE  ProcessHandle,
    _Out_ PDWORD HandleCount
    );

_IRQL_requires_max_(PASSIVE_LEVEL)
BOOL WINAPI MUSA_NAME(GetProcessPriorityBoost)(
    _In_ HANDLE ProcessHandle,
    _Out_ PBOOL DisablePriorityBoost
    );

_IRQL_requires_max_(PASSIVE_LEVEL)
BOOL WINAPI MUSA_NAME(SetProcessPriorityBoost)(
    _In_ HANDLE ProcessHandle,
    _In_ BOOL   DisablePriorityBoost
    );

_IRQL_requires_max_(PASSIVE_LEVEL)
BOOL WINAPI MUSA_NAME(IsProcessCritical)(
    _In_ HANDLE ProcessHandle,
    _Out_ PBOOL Critical
    );

_IRQL_requires_max_(PASSIVE_LEVEL)
BOOL WINAPI MUSA_NAME(SetProcessInformation)(
    _In_ HANDLE                                     ProcessHandle,
    _In_ PROCESS_INFORMATION_CLASS                  ProcessInformationClass,
    _In_reads_bytes_(ProcessInformationSize) LPVOID ProcessInformation,
    _In_ DWORD                                      ProcessInformationSize
    );

_IRQL_requires_max_(PASSIVE_LEVEL)
BOOL WINAPI MUSA_NAME(GetProcessInformation)(
    _In_ HANDLE                                       ProcessHandle,
    _In_ PROCESS_INFORMATION_CLASS                    ProcessInformationClass,
    _Out_writes_bytes_(ProcessInformationSize) LPVOID ProcessInformation,
    _In_ DWORD                                        ProcessInformationSize
    );

EXTERN_C_END
