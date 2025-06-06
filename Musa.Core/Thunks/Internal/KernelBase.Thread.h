﻿#pragma once

EXTERN_C_START

_IRQL_requires_max_(PASSIVE_LEVEL)
BOOL WINAPI MUSA_NAME(SwitchToThread)();

_IRQL_requires_max_(PASSIVE_LEVEL)
HANDLE WINAPI MUSA_NAME(CreateThread)(
    _In_opt_ LPSECURITY_ATTRIBUTES   ThreadAttributes,
    _In_ SIZE_T                      StackSize,
    _In_ LPTHREAD_START_ROUTINE      StartAddress,
    _In_opt_ __drv_aliasesMem LPVOID Parameter,
    _In_ DWORD                       CreationFlags,
    _Out_opt_ LPDWORD                ThreadId
    );

_IRQL_requires_max_(PASSIVE_LEVEL)
HANDLE WINAPI MUSA_NAME(CreateRemoteThread)(
    _In_ HANDLE                    Process,
    _In_opt_ LPSECURITY_ATTRIBUTES ThreadAttributes,
    _In_ SIZE_T                    StackSize,
    _In_ LPTHREAD_START_ROUTINE    StartAddress,
    _In_opt_ LPVOID                Parameter,
    _In_ DWORD                     CreationFlags,
    _Out_opt_ LPDWORD              ThreadId
    );

_IRQL_requires_max_(PASSIVE_LEVEL)
HANDLE WINAPI MUSA_NAME(CreateRemoteThreadEx)(
    _In_ HANDLE                           Process,
    _In_opt_ LPSECURITY_ATTRIBUTES        ThreadAttributes,
    _In_ SIZE_T                           StackSize,
    _In_ LPTHREAD_START_ROUTINE           StartAddress,
    _In_opt_ LPVOID                       Parameter,
    _In_ DWORD                            CreationFlags,
    _In_opt_ LPPROC_THREAD_ATTRIBUTE_LIST AttributeList,
    _Out_opt_ LPDWORD                     ThreadId
    );

_IRQL_requires_max_(PASSIVE_LEVEL)
VOID WINAPI MUSA_NAME(ExitThread)(
    _In_ DWORD ExitCode
    );

_IRQL_requires_max_(PASSIVE_LEVEL)
HANDLE WINAPI MUSA_NAME(OpenThread)(
    _In_ DWORD DesiredAccess,
    _In_ BOOL  InheritHandle,
    _In_ DWORD ThreadId
    );

_IRQL_requires_max_(PASSIVE_LEVEL)
BOOL WINAPI MUSA_NAME(SetThreadPriority)(
    _In_ HANDLE ThreadHandle,
    _In_ int    Priority
    );

_IRQL_requires_max_(PASSIVE_LEVEL)
int WINAPI MUSA_NAME(GetThreadPriority)(
    _In_ HANDLE ThreadHandle
    );

_IRQL_requires_max_(PASSIVE_LEVEL)
BOOL WINAPI MUSA_NAME(SetThreadPriorityBoost)(
    _In_ HANDLE ThreadHandle,
    _In_ BOOL   DisablePriorityBoost
    );

_IRQL_requires_max_(PASSIVE_LEVEL)
BOOL WINAPI MUSA_NAME(GetThreadPriorityBoost)(
    _In_ HANDLE ThreadHandle,
    _Out_ PBOOL DisablePriorityBoost
    );

_IRQL_requires_max_(PASSIVE_LEVEL)
BOOL WINAPI MUSA_NAME(TerminateThread)(
    _In_ HANDLE ThreadHandle,
    _In_ DWORD  ExitCode
    );

_IRQL_requires_max_(PASSIVE_LEVEL)
BOOL WINAPI MUSA_NAME(GetExitCodeThread)(
    _In_ HANDLE   ThreadHandle,
    _Out_ LPDWORD ExitCode
    );

_IRQL_requires_max_(PASSIVE_LEVEL)
DWORD WINAPI MUSA_NAME(SuspendThread)(
    _In_ HANDLE ThreadHandle
    );

_IRQL_requires_max_(PASSIVE_LEVEL)
DWORD WINAPI MUSA_NAME(ResumeThread)(
    _In_ HANDLE ThreadHandle
    );

_IRQL_requires_max_(PASSIVE_LEVEL)
BOOL WINAPI MUSA_NAME(SetThreadToken)(
    _In_opt_ PHANDLE ThreadHandle,
    _In_opt_ HANDLE  Token
    );

_IRQL_requires_max_(PASSIVE_LEVEL)
BOOL WINAPI MUSA_NAME(OpenThreadToken)(
    _In_ HANDLE      ThreadHandle,
    _In_ DWORD       DesiredAccess,
    _In_ BOOL        OpenAsSelf,
    _Outptr_ PHANDLE TokenHandle
    );

_IRQL_requires_max_(PASSIVE_LEVEL)
DWORD WINAPI MUSA_NAME(GetThreadId)(
    _In_ HANDLE ThreadHandle
    );

_IRQL_requires_max_(APC_LEVEL)
VOID WINAPI MUSA_NAME(GetCurrentThreadStackLimits)(
    _Out_ PULONG_PTR LowLimit,
    _Out_ PULONG_PTR HighLimit
    );

_IRQL_requires_max_(PASSIVE_LEVEL)
BOOL WINAPI MUSA_NAME(GetThreadContext)(
    _In_ HANDLE       ThreadHandle,
    _Inout_ LPCONTEXT Context
    );

_IRQL_requires_max_(PASSIVE_LEVEL)
BOOL WINAPI MUSA_NAME(SetThreadContext)(
    _In_ HANDLE         ThreadHandle,
    _In_ CONST CONTEXT* Context
    );

_IRQL_requires_max_(PASSIVE_LEVEL)
BOOL WINAPI MUSA_NAME(GetThreadTimes)(
    _In_ HANDLE      ThreadHandle,
    _Out_ LPFILETIME CreationTime,
    _Out_ LPFILETIME ExitTime,
    _Out_ LPFILETIME KernelTime,
    _Out_ LPFILETIME UserTime
    );

_IRQL_requires_max_(PASSIVE_LEVEL)
DWORD WINAPI MUSA_NAME(SetThreadIdealProcessor)(
    _In_ HANDLE ThreadHandle,
    _In_ DWORD  IdealProcessor
    );

_IRQL_requires_max_(PASSIVE_LEVEL)
BOOL WINAPI MUSA_NAME(SetThreadIdealProcessorEx)(
    _In_ HANDLE                 ThreadHandle,
    _In_ PPROCESSOR_NUMBER      IdealProcessor,
    _Out_opt_ PPROCESSOR_NUMBER PreviousIdealProcessor
    );

_IRQL_requires_max_(PASSIVE_LEVEL)
BOOL WINAPI MUSA_NAME(GetThreadIdealProcessorEx)(
    _In_ HANDLE            ThreadHandle,
    _In_ PPROCESSOR_NUMBER IdealProcessor
    );

_IRQL_requires_max_(PASSIVE_LEVEL)
BOOL WINAPI MUSA_NAME(GetThreadIOPendingFlag)(
    _In_ HANDLE ThreadHandle,
    _Out_ PBOOL IOIsPending
    );

_IRQL_requires_max_(PASSIVE_LEVEL)
BOOL WINAPI MUSA_NAME(GetThreadInformation)(
    _In_ HANDLE                                      ThreadHandle,
    _In_ THREAD_INFORMATION_CLASS                    ThreadInformationClass,
    _Out_writes_bytes_(ThreadInformationSize) LPVOID ThreadInformation,
    _In_ DWORD                                       ThreadInformationSize
    );

_IRQL_requires_max_(PASSIVE_LEVEL)
BOOL WINAPI MUSA_NAME(SetThreadInformation)(
    _In_ HANDLE                                    ThreadHandle,
    _In_ THREAD_INFORMATION_CLASS                  ThreadInformationClass,
    _In_reads_bytes_(ThreadInformationSize) LPVOID ThreadInformation,
    _In_ DWORD                                     ThreadInformationSize
    );

_IRQL_requires_max_(PASSIVE_LEVEL)
HRESULT WINAPI MUSA_NAME(SetThreadDescription)(
    _In_ HANDLE ThreadHandle,
    _In_ PCWSTR ThreadDescription
    );

_IRQL_requires_max_(PASSIVE_LEVEL)
HRESULT WINAPI MUSA_NAME(GetThreadDescription)(
    _In_ HANDLE              ThreadHandle,
    _Outptr_result_z_ PWSTR* ThreadDescription
    );

EXTERN_C_END
