﻿#include "Musa.Core/Musa.Core.SystemEnvironmentBlock.Fls.h"
#include "Musa.Core/Musa.Core.SystemEnvironmentBlock.Thread.h"
#include "Internal/Ntdll.FiberLocalStorage.h"

#ifdef ALLOC_PRAGMA
#pragma alloc_text(PAGE, MUSA_NAME(RtlProcessFlsData))
#endif

EXTERN_C_START

#if defined(_KERNEL_MODE)

using namespace Musa;
using namespace Musa::Core;

namespace Musa::Core
{
    extern RTL_FLS_CONTEXT RtlFlsContext;
}

_IRQL_requires_max_(APC_LEVEL)
VOID NTAPI MUSA_NAME(RtlProcessFlsData)(
    _In_ PVOID FlsData,
    _In_ ULONG Flags
)
{
    PAGED_CODE();
    return MUSA_NAME_PRIVATE(FlsDataCleanup)(FlsData, Flags);
}

MUSA_IAT_SYMBOL(RtlProcessFlsData, 8);

_IRQL_requires_max_(DISPATCH_LEVEL)
NTSTATUS NTAPI MUSA_NAME(RtlFlsAlloc)(
    _In_ PFLS_CALLBACK_FUNCTION Callback,
    _Out_ PULONG                FlsIndex
)
{
    *FlsIndex = FLS_OUT_OF_INDEXES;

    const auto LockIrql = ExAcquireSpinLockExclusive(&RtlFlsContext.Lock);
    __try {
        //
        // Search for the first free entry in the fiber local storage
        // bitmap.
        //

        const auto Index = RtlFindClearBitsAndSet(&RtlFlsContext.FlsBitmap, 1, 0);

        //
        // If no fiber local storage is available, then set the last error
        // value and return the distinguished value.
        //

        if (Index == FLS_OUT_OF_INDEXES) {
            return STATUS_NO_MEMORY;
        }

        //
        // Initialize the free function callback address and the fiber
        // local storage value.
        //

        if (Callback) {
            if (InterlockedCompareExchangePointer(reinterpret_cast<PVOID volatile*>(&RtlFlsContext.FlsCallback),
                nullptr, nullptr) == nullptr) {
                const auto FlsCallback = static_cast<PFLS_CALLBACK_FUNCTION*>(
                    RtlAllocateHeap(RtlProcessHeap(), HEAP_ZERO_MEMORY,
                        FLS_MAXIMUM_AVAILABLE * sizeof(PFLS_CALLBACK_FUNCTION)));

                if (FlsCallback == nullptr) {
                    RtlClearBits(&RtlFlsContext.FlsBitmap, Index, 1);
                    return STATUS_NO_MEMORY;
                }

                if (InterlockedCompareExchangePointer(reinterpret_cast<PVOID volatile*>(&RtlFlsContext.FlsCallback),
                    (PVOID)FlsCallback, nullptr)) {
                    RtlFreeHeap(RtlProcessHeap(), 0, (PVOID)FlsCallback);
                }
            }

            RtlFlsContext.FlsCallback[Index] = Callback;
        }

        //
        // If the allocated index is greater than the highest allocated
        // index, then save the new high.
        //

        if (Index > RtlFlsContext.FlsHighIndex) {
            RtlFlsContext.FlsHighIndex = Index;
        }

        *FlsIndex = Index;
    } __finally {
        ExReleaseSpinLockExclusive(&RtlFlsContext.Lock, LockIrql);
    }

    return STATUS_SUCCESS;
}

MUSA_IAT_SYMBOL(RtlFlsAlloc, 8);

_IRQL_requires_max_(DISPATCH_LEVEL)
NTSTATUS NTAPI MUSA_NAME(RtlFlsFree)(
    _In_ ULONG FlsIndex
)
{
    if (FlsIndex >= RTL_FLS_MAXIMUM_AVAILABLE) {
        return STATUS_INVALID_PARAMETER;
    }

    //
    // If the fiber local storage index is invalid, then return FALSE.
    // Otherwise, invoke the callback function associated with the fiber
    // local storage data entry if necessary, free the specified index,
    // and return TRUE.
    //

    auto LockIrql = ExAcquireSpinLockExclusive(&RtlFlsContext.Lock);
    __try {
        if (!RtlAreBitsSet(&RtlFlsContext.FlsBitmap, FlsIndex, 1)) {
            return STATUS_INVALID_PARAMETER;
        }

        //
        // Clear the allocated slot in the fiber local storage bitmap,
        // enumerate fiber local data structures, and invoke callback
        // routines if necessary.
        //

        RtlClearBits(&RtlFlsContext.FlsBitmap, FlsIndex, 1);

        const auto FlsCallback = static_cast<PFLS_CALLBACK_FUNCTION*>(InterlockedCompareExchangePointer(
            reinterpret_cast<PVOID volatile*>(&RtlFlsContext.FlsCallback), nullptr, nullptr));

        if (FlsCallback) {
            auto Entry = RtlFlsContext.FlsListHead.Flink;

            while (Entry != &RtlFlsContext.FlsListHead) {
                const auto FlsData = CONTAINING_RECORD(Entry, RTL_FLS_DATA, Entry);

                const auto Callback = FlsCallback[FlsIndex];
                if (Callback) {
                    if (FlsData->Slots[FlsIndex]) {
                        ExReleaseSpinLockExclusive(&RtlFlsContext.Lock, LockIrql);
                        __try {
                            Callback(FlsData->Slots[FlsIndex]);
                        } __finally {
                            LockIrql = ExAcquireSpinLockExclusive(&RtlFlsContext.Lock);
                        }
                    }
                }
                FlsData->Slots[FlsIndex] = nullptr;

                Entry = Entry->Flink;
            }
        }
    } __finally {
        ExReleaseSpinLockExclusive(&RtlFlsContext.Lock, LockIrql);
    }

    return STATUS_SUCCESS;
}

MUSA_IAT_SYMBOL(RtlFlsFree, 4);

_IRQL_requires_max_(DISPATCH_LEVEL)
NTSTATUS NTAPI MUSA_NAME(RtlFlsGetValue)(
    _In_ ULONG   FlsIndex,
    _Out_ PVOID* FlsData
)
{
    *FlsData = nullptr;

    if (FlsIndex >= RTL_FLS_MAXIMUM_AVAILABLE) {
        return STATUS_INVALID_PARAMETER;
    }

    //
    // If the fiber local storage index is invalid or the fiber local storage
    // data structure has not been allocated, then set the last error value
    // and return NULL. Otherwise, return the specified value from fiber local
    // storage data.
    //

    if (FlsIndex > RtlFlsContext.FlsHighIndex) {
        return STATUS_INVALID_PARAMETER;
    }

    if (!RtlAreBitsSet(&RtlFlsContext.FlsBitmap, FlsIndex, 1)) {
        return STATUS_INVALID_PARAMETER;
    }

    const auto Teb = (PKTEB)MUSA_NAME_PRIVATE(RtlGetCurrentTeb)();
    if (Teb == nullptr) {
        return STATUS_MEMORY_NOT_ALLOCATED;
    }

    if (Teb->FlsData == nullptr) {
        return STATUS_MEMORY_NOT_ALLOCATED;
    }

    *FlsData = Teb->FlsData->Slots[FlsIndex];

    return STATUS_SUCCESS;
}

MUSA_IAT_SYMBOL(RtlFlsGetValue, 8);

#if defined _M_IX86
_VEIL_DEFINE_IAT_RAW_SYMBOL(RtlFlsGetValue2@4, MUSA_NAME(RtlFlsGetValue));
#else
_VEIL_DEFINE_IAT_SYMBOL(RtlFlsGetValue2, MUSA_NAME(RtlFlsGetValue));
#endif

_IRQL_requires_max_(DISPATCH_LEVEL)
NTSTATUS NTAPI MUSA_NAME(RtlFlsSetValue)(
    _In_ ULONG FlsIndex,
    _In_ PVOID FlsData
)
{
    if (FlsIndex >= RTL_FLS_MAXIMUM_AVAILABLE) {
        return STATUS_INVALID_PARAMETER;
    }

    //
    // If the fiber local storage index is invalid, then set the last error
    // value and return FALSE. Otherwise, attempt to store the specified
    // value in the associated fiber local storage data.
    //

    if (FlsIndex > RtlFlsContext.FlsHighIndex) {
        return STATUS_INVALID_PARAMETER;
    }

    if (!RtlAreBitsSet(&RtlFlsContext.FlsBitmap, FlsIndex, 1)) {
        return STATUS_INVALID_PARAMETER;
    }

    //
    // If the fiber local storage data structure has not yet been
    // allocated for the current thread, then attempt to allocate it
    // now.
    //

    const auto Teb = (PKTEB)MUSA_NAME_PRIVATE(RtlGetCurrentTeb)();
    if (Teb == nullptr) {
        return STATUS_MEMORY_NOT_ALLOCATED;
    }

    if (Teb->FlsData == nullptr) {
        Teb->FlsData = static_cast<PRTL_FLS_DATA>(RtlAllocateHeap(RtlProcessHeap(),
            HEAP_ZERO_MEMORY, sizeof(RTL_FLS_DATA)));

        //
        // If a fiber local storage data structure was allocated, then
        // insert the allocated structure in the process fiber local
        // storage list. Otherwise, clear the allocated slot in the bitmap,
        // set the last error value, return the distinguished value.
        //

        if (Teb->FlsData == nullptr) {
            return STATUS_NO_MEMORY;
        }

        const auto LockIrql = ExAcquireSpinLockExclusive(&RtlFlsContext.Lock);
        __try {
            InsertTailList(&RtlFlsContext.FlsListHead, &Teb->FlsData->Entry);
        } __finally {
            ExReleaseSpinLockExclusive(&RtlFlsContext.Lock, LockIrql);
        }
    }

    //
    // Store the specified value in the fiber local storage data entry and
    // return STATUS_SUCCESS.
    //

    Teb->FlsData->Slots[FlsIndex] = FlsData;

    return STATUS_SUCCESS;
}

MUSA_IAT_SYMBOL(RtlFlsSetValue, 8);

#endif // defined(_KERNEL_MODE)

EXTERN_C_END
