﻿#pragma once

EXTERN_C_START

//
// R/W lock
//

_IRQL_requires_max_(APC_LEVEL)
_Acquires_exclusive_lock_(*SRWLock)
VOID NTAPI MUSA_NAME(RtlAcquireSRWLockExclusive)(
    _Inout_ PRTL_SRWLOCK SRWLock
    );

_IRQL_requires_max_(APC_LEVEL)
_Acquires_shared_lock_(*SRWLock)
VOID NTAPI MUSA_NAME(RtlAcquireSRWLockShared)(
    _Inout_ PRTL_SRWLOCK SRWLock
    );

_IRQL_requires_max_(APC_LEVEL)
_Releases_exclusive_lock_(*SRWLock)
VOID NTAPI MUSA_NAME(RtlReleaseSRWLockExclusive)(
    _Inout_ PRTL_SRWLOCK SRWLock
    );

_IRQL_requires_max_(APC_LEVEL)
_Releases_shared_lock_(*SRWLock)
VOID NTAPI MUSA_NAME(RtlReleaseSRWLockShared)(
    _Inout_ PRTL_SRWLOCK SRWLock
    );

_IRQL_requires_max_(APC_LEVEL)
_When_(return != 0, _Acquires_exclusive_lock_(*SRWLock))
BOOLEAN NTAPI MUSA_NAME(RtlTryAcquireSRWLockExclusive)(
    _Inout_ PRTL_SRWLOCK SRWLock
    );

_IRQL_requires_max_(APC_LEVEL)
_When_(return != 0, _Acquires_shared_lock_(*SRWLock))
BOOLEAN NTAPI MUSA_NAME(RtlTryAcquireSRWLockShared)(
    _Inout_ PRTL_SRWLOCK SRWLock
    );

//
// Critical Section
//

_IRQL_requires_max_(APC_LEVEL)
_Acquires_exclusive_lock_(*CriticalSection)
NTSTATUS NTAPI MUSA_NAME(RtlEnterCriticalSection)(
    _Inout_ PRTL_CRITICAL_SECTION CriticalSection
    );

_IRQL_requires_max_(APC_LEVEL)
_Releases_exclusive_lock_(*CriticalSection)
NTSTATUS NTAPI MUSA_NAME(RtlLeaveCriticalSection)(
    _Inout_ PRTL_CRITICAL_SECTION CriticalSection
    );

_IRQL_requires_max_(APC_LEVEL)
_When_(return != 0, _Acquires_exclusive_lock_(*CriticalSection))
LOGICAL NTAPI MUSA_NAME(RtlTryEnterCriticalSection)(
    _Inout_ PRTL_CRITICAL_SECTION CriticalSection
    );

//
// Condition variable
//

_IRQL_requires_max_(PASSIVE_LEVEL)
NTSTATUS NTAPI MUSA_NAME(RtlSleepConditionVariableCS)(
    _Inout_ PRTL_CONDITION_VARIABLE ConditionVariable,
    _Inout_ PRTL_CRITICAL_SECTION   CriticalSection,
    _In_opt_ PLARGE_INTEGER         Timeout
    );

_IRQL_requires_max_(PASSIVE_LEVEL)
NTSTATUS NTAPI MUSA_NAME(RtlSleepConditionVariableSRW)(
    _Inout_ PRTL_CONDITION_VARIABLE ConditionVariable,
    _Inout_ PRTL_SRWLOCK            SRWLock,
    _In_opt_ PLARGE_INTEGER         Timeout,
    _In_ ULONG                      Flags
    );

_IRQL_requires_max_(PASSIVE_LEVEL)
VOID NTAPI MUSA_NAME(RtlWakeConditionVariable)(
    _Inout_ PRTL_CONDITION_VARIABLE ConditionVariable
    );

_IRQL_requires_max_(PASSIVE_LEVEL)
VOID NTAPI MUSA_NAME(RtlWakeAllConditionVariable)(
    _Inout_ PRTL_CONDITION_VARIABLE ConditionVariable
    );

EXTERN_C_END
