﻿#include "KernelBase.Private.h"

EXTERN_C_START

_IRQL_requires_max_(DISPATCH_LEVEL)
HANDLE WINAPI MUSA_NAME(GetProcessHeap)(
    VOID
)
{
    return RtlProcessHeap();
}

MUSA_IAT_SYMBOL(GetProcessHeap, 0);

_IRQL_requires_max_(APC_LEVEL)
DWORD WINAPI MUSA_NAME(GetProcessHeaps)(
    _In_ DWORD                                     NumberOfHeaps,
    _Out_writes_to_(NumberOfHeaps, return) PHANDLE ProcessHeaps
)
{
    return RtlGetProcessHeaps(NumberOfHeaps, ProcessHeaps);
}

MUSA_IAT_SYMBOL(GetProcessHeaps, 8);

_IRQL_requires_max_(APC_LEVEL)
_Ret_maybenull_
HANDLE WINAPI MUSA_NAME(HeapCreate)(
    _In_ DWORD  Options,
    _In_ SIZE_T InitialSize,
    _In_ SIZE_T MaximumSize
)
{
    return RtlCreateHeap(Options, nullptr,
        MaximumSize, InitialSize, nullptr, nullptr);
}

MUSA_IAT_SYMBOL(HeapCreate, 12);

_IRQL_requires_max_(APC_LEVEL)
BOOL WINAPI MUSA_NAME(HeapDestroy)(
    _In_ HANDLE HeapHandle
)
{
    return (RtlDestroyHeap(HeapHandle) == nullptr);
}

MUSA_IAT_SYMBOL(HeapDestroy, 4);

_IRQL_requires_max_(DISPATCH_LEVEL)
_Ret_maybenull_
_Post_writable_byte_size_(Bytes)
DECLSPEC_ALLOCATOR
LPVOID WINAPI MUSA_NAME(HeapAlloc)(
    _In_ HANDLE HeapHandle,
    _In_ DWORD  Flags,
    _In_ SIZE_T Bytes
)
{
    return RtlAllocateHeap(HeapHandle, Flags, Bytes);
}

MUSA_IAT_SYMBOL(HeapAlloc, 12);

_IRQL_requires_max_(DISPATCH_LEVEL)
_Success_(return != 0)
_Ret_maybenull_
_Post_writable_byte_size_(Bytes)
DECLSPEC_ALLOCATOR
LPVOID WINAPI MUSA_NAME(HeapReAlloc)(
    _Inout_ HANDLE         HeapHandle,
    _In_ DWORD             Flags,
    _Frees_ptr_opt_ LPVOID Mem,
    _In_ SIZE_T            Bytes
)
{
    return RtlReAllocateHeap(HeapHandle, Flags, Mem, Bytes);
}

MUSA_IAT_SYMBOL(HeapReAlloc, 16);

_IRQL_requires_max_(DISPATCH_LEVEL)
_Success_(return != FALSE)
BOOL WINAPI MUSA_NAME(HeapFree)(
    _Inout_ HANDLE                             HeapHandle,
    _In_ DWORD                                 Flags,
    __drv_freesMem(Mem) _Frees_ptr_opt_ LPVOID Mem
)
{
    return !!RtlFreeHeap(HeapHandle, Flags, Mem);
}

MUSA_IAT_SYMBOL(HeapFree, 12);

_IRQL_requires_max_(DISPATCH_LEVEL)
SIZE_T WINAPI MUSA_NAME(HeapSize)(
    _In_ HANDLE  HeapHandle,
    _In_ DWORD   Flags,
    _In_ LPCVOID Mem
)
{
    return RtlSizeHeap(HeapHandle, Flags, const_cast<PVOID>(Mem));
}

MUSA_IAT_SYMBOL(HeapSize, 12);

SIZE_T WINAPI MUSA_NAME(HeapCompact)(
    _In_ HANDLE HeapHandle,
    _In_ DWORD  Flags
)
{
    return RtlCompactHeap(HeapHandle, Flags);
}

MUSA_IAT_SYMBOL(HeapCompact, 8);

BOOL WINAPI MUSA_NAME(HeapLock)(
    _In_ HANDLE HeapHandle
)
{
    return RtlLockHeap(HeapHandle);
}

MUSA_IAT_SYMBOL(HeapLock, 4);

BOOL WINAPI MUSA_NAME(HeapUnlock)(
    _In_ HANDLE HeapHandle
)
{
    return RtlUnlockHeap(HeapHandle);
}

MUSA_IAT_SYMBOL(HeapUnlock, 4);

BOOL WINAPI MUSA_NAME(HeapValidate)(
    _In_ HANDLE      HeapHandle,
    _In_ DWORD       Flags,
    _In_opt_ LPCVOID Mem
)
{
    return RtlValidateHeap(HeapHandle, Flags, Mem);
}

MUSA_IAT_SYMBOL(HeapValidate, 12);

static
_Function_class_(RTL_HEAP_EXTENDED_ENUMERATION_ROUTINE)
NTSTATUS NTAPI HeapSummaryWorkerRoutine(
    _In_ PHEAP_INFORMATION_ITEM Information,
    _In_opt_ PVOID              Context
)
{
    const auto Summary = static_cast<LPHEAP_SUMMARY>(Context);
    if (Summary == nullptr) {
        return STATUS_SUCCESS;
    }

    switch (Information->Level) {
        default: {
            break;
        }
        case HeapExtendedHeapRegionInformationLevel: {
            Summary->cbReserved += Information->HeapRegionInformation.ReserveSize;
            Summary->cbMaxReserve += Information->HeapRegionInformation.ReserveSize;
            break;
        }
        case HeapExtendedHeapRangeInformationLevel: {
            if (Information->HeapRangeInformation.Type == 1) {
                Summary->cbCommitted += Information->HeapRangeInformation.Size;
            }
            break;
        }
        case HeapExtendedHeapBlockInformationLevel: {
            Summary->cbAllocated += Information->HeapBlockInformation.DataSize;
            break;
        }
    }

    return STATUS_SUCCESS;
}

BOOL WINAPI MUSA_NAME(HeapSummary)(
    _In_ HANDLE            HeapHandle,
    _In_ DWORD             Flags,
    _Inout_ LPHEAP_SUMMARY Summary
)
{
    UNREFERENCED_PARAMETER(Flags);

    if (Summary->cb != sizeof(*Summary)) {
        BaseSetLastNTError(STATUS_INVALID_PARAMETER);
        return FALSE;
    }

    Summary->cbAllocated  = 0;
    Summary->cbCommitted  = 0;
    Summary->cbReserved   = 0;
    Summary->cbMaxReserve = 0;

    HEAP_EXTENDED_INFORMATION SummaryInformation{};
    SummaryInformation.ProcessHandle   = ZwCurrentProcess();
    SummaryInformation.HeapHandle      = HeapHandle;
    SummaryInformation.Level           = HeapExtendedHeapBlockInformationLevel;
    SummaryInformation.CallbackRoutine = HeapSummaryWorkerRoutine;
    SummaryInformation.CallbackContext = Summary;

    const auto Status = RtlQueryHeapInformation(HeapHandle, HeapExtendedInformation,
        &SummaryInformation, sizeof(SummaryInformation), nullptr);
    if (NT_SUCCESS(Status)) {
        return TRUE;
    }

    BaseSetLastNTError(Status);
    return FALSE;
}

MUSA_IAT_SYMBOL(HeapSummary, 12);

BOOL WINAPI MUSA_NAME(HeapWalk)(
    _In_ HANDLE                  HeapHandle,
    _Inout_ LPPROCESS_HEAP_ENTRY Entry
)
{
    const auto Status = RtlWalkHeap(HeapHandle,
        reinterpret_cast<PRTL_HEAP_WALK_ENTRY>(Entry));
    if (NT_SUCCESS(Status)) {
        return TRUE;
    }

    BaseSetLastNTError(Status);
    return FALSE;
}

MUSA_IAT_SYMBOL(HeapWalk, 8);

BOOL WINAPI MUSA_NAME(HeapSetInformation)(
    _In_opt_ HANDLE                                   HeapHandle,
    _In_ HEAP_INFORMATION_CLASS                       HeapInformationClass,
    _In_reads_bytes_opt_(HeapInformationLength) PVOID HeapInformation,
    _In_ SIZE_T                                       HeapInformationLength
)
{
    const auto Status = RtlSetHeapInformation(HeapHandle,
        HeapInformationClass, HeapInformation, HeapInformationLength);
    if (NT_SUCCESS(Status)) {
        return TRUE;
    }

    BaseSetLastNTError(Status);
    return FALSE;
}

MUSA_IAT_SYMBOL(HeapSetInformation, 16);

BOOL WINAPI MUSA_NAME(HeapQueryInformation)(
    _In_opt_ HANDLE                                                       HeapHandle,
    _In_ HEAP_INFORMATION_CLASS                                           HeapInformationClass,
    _Out_writes_bytes_to_opt_(HeapInformationLength, *ReturnLength) PVOID HeapInformation,
    _In_ SIZE_T                                                           HeapInformationLength,
    _Out_opt_ PSIZE_T                                                     ReturnLength
)
{
    const auto Status = RtlQueryHeapInformation(HeapHandle,
        HeapInformationClass, HeapInformation, HeapInformationLength, ReturnLength);
    if (NT_SUCCESS(Status)) {
        return TRUE;
    }

    BaseSetLastNTError(Status);
    return FALSE;
}

MUSA_IAT_SYMBOL(HeapQueryInformation, 20);


//
// Local
//

HLOCAL WINAPI MUSA_NAME(LocalAlloc)(
    _In_ UINT   Flags,
    _In_ SIZE_T Bytes
)
{
    DWORD HeapFlags = 0;

    if (BooleanFlagOn(Flags, LMEM_MOVEABLE)) {
        BaseSetLastNTError(STATUS_NOT_IMPLEMENTED);
        return nullptr;
    }

    if (BooleanFlagOn(Flags, LMEM_ZEROINIT)) {
        HeapFlags |= HEAP_ZERO_MEMORY;
    }

    return HeapAlloc(GetProcessHeap(), HeapFlags, Bytes);
}

MUSA_IAT_SYMBOL(LocalAlloc, 8);

HLOCAL WINAPI MUSA_NAME(LocalReAlloc)(
    _Frees_ptr_opt_ HLOCAL Mem,
    _In_ SIZE_T            Bytes,
    _In_ UINT              Flags
)
{
    DWORD HeapFlags = 0;

    if (BooleanFlagOn(Flags, LMEM_MOVEABLE)) {
        BaseSetLastNTError(STATUS_NOT_IMPLEMENTED);
        return nullptr;
    }

    if (BooleanFlagOn(Flags, LMEM_ZEROINIT)) {
        HeapFlags |= HEAP_ZERO_MEMORY;
    }

    return HeapReAlloc(GetProcessHeap(), HeapFlags, Mem, Bytes);
}

MUSA_IAT_SYMBOL(LocalReAlloc, 12);

HLOCAL WINAPI MUSA_NAME(LocalFree)(
    _Frees_ptr_opt_ HLOCAL Mem
)
{
    if (HeapFree(GetProcessHeap(), 0, Mem)) {
        return nullptr;
    }

    return Mem;
}

MUSA_IAT_SYMBOL(LocalFree, 4);

LPVOID WINAPI MUSA_NAME(LocalLock)(
    _In_ HLOCAL Mem
)
{
    return Mem;
}

MUSA_IAT_SYMBOL(LocalLock, 4);

BOOL WINAPI MUSA_NAME(LocalUnlock)(
    _In_ HLOCAL Mem
)
{
    UNREFERENCED_PARAMETER(Mem);
    return TRUE;
}

MUSA_IAT_SYMBOL(LocalUnlock, 4);

HLOCAL WINAPI MUSA_NAME(LocalHandle)(
    _In_ LPCVOID Mem
)
{
    return const_cast<HLOCAL>(Mem);
}

MUSA_IAT_SYMBOL(LocalHandle, 4);

SIZE_T WINAPI MUSA_NAME(LocalSize)(
    _In_ HLOCAL Mem
)
{
    return HeapSize(GetProcessHeap(), 0, Mem);
}

MUSA_IAT_SYMBOL(LocalSize, 4);

UINT WINAPI MUSA_NAME(LocalFlags)(
    _In_ HLOCAL Mem
)
{
    UNREFERENCED_PARAMETER(Mem);
    return 0;
}

MUSA_IAT_SYMBOL(LocalFlags, 4);


//
// Global
//

HGLOBAL WINAPI MUSA_NAME(GlobalAlloc)(
    _In_ UINT   Flags,
    _In_ SIZE_T Bytes
)
{
    return LocalAlloc(Flags, Bytes);
}

MUSA_IAT_SYMBOL(GlobalAlloc, 8);

HGLOBAL WINAPI MUSA_NAME(GlobalReAlloc)(
    _Frees_ptr_opt_ HGLOBAL Mem,
    _In_ SIZE_T             Bytes,
    _In_ UINT               Flags
)
{
    return LocalReAlloc(Mem, Bytes, Flags);
}

MUSA_IAT_SYMBOL(GlobalReAlloc, 12);

HGLOBAL WINAPI MUSA_NAME(GlobalFree)(
    _Frees_ptr_opt_ HGLOBAL Mem
)
{
    return LocalFree(Mem);
}

MUSA_IAT_SYMBOL(GlobalFree, 4);

LPVOID WINAPI MUSA_NAME(GlobalLock)(
    _In_ HGLOBAL Mem
)
{
    return LocalLock(Mem);
}

MUSA_IAT_SYMBOL(GlobalLock, 4);

BOOL WINAPI MUSA_NAME(GlobalUnlock)(
    _In_ HGLOBAL Mem
)
{
    return LocalUnlock(Mem);
}

MUSA_IAT_SYMBOL(GlobalUnlock, 4);

HGLOBAL WINAPI MUSA_NAME(GlobalHandle)(
    _In_ LPCVOID Mem
)
{
    return LocalHandle(Mem);
}

MUSA_IAT_SYMBOL(GlobalHandle, 4);

SIZE_T WINAPI MUSA_NAME(GlobalSize)(
    _In_ HGLOBAL Mem
)
{
    return LocalSize(Mem);
}

MUSA_IAT_SYMBOL(GlobalSize, 4);

UINT WINAPI MUSA_NAME(GlobalFlags)(
    _In_ HGLOBAL Mem
)
{
    return LocalFlags(Mem);
}

MUSA_IAT_SYMBOL(GlobalFlags, 4);

EXTERN_C_END
