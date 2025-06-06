﻿#include "Musa.Utilities.h"
#include "Musa.Core.SystemEnvironmentBlock.Fls.h"
#include "Musa.Core.SystemEnvironmentBlock.Process.h"
#include "Musa.Core.SystemEnvironmentBlock.Thread.h"

#ifdef ALLOC_PRAGMA
#pragma alloc_text(INIT, MUSA_NAME_PRIVATE(ThreadEnvironmentBlockSetup))
#pragma alloc_text(PAGE, MUSA_NAME_PRIVATE(ThreadEnvironmentBlockTeardown))
#endif

EXTERN_C_START

#if defined(_KERNEL_MODE)

using namespace Musa;
using namespace Musa::Core;

namespace Musa::Core
{
    namespace
    {
        EX_SPIN_LOCK  MusaCoreTebLock{};
        RTL_AVL_TABLE MusaCoreTebTable{};
        LOOKASIDE_ALIGN LOOKASIDE_LIST_EX MusaCoreTebPool{};
        PVOID MusaCoreThreadNotifyCallbackHandleForTeb = nullptr;

        namespace TebTableRoutines
        {
            RTL_AVL_COMPARE_ROUTINE  Compare;
            RTL_AVL_ALLOCATE_ROUTINE Allocate;
            RTL_AVL_FREE_ROUTINE     Free;

            RTL_GENERIC_COMPARE_RESULTS NTAPI Compare(
                _In_ RTL_AVL_TABLE* Table,
                _In_ PVOID First,
                _In_ PVOID Second
            )
            {
                UNREFERENCED_PARAMETER(Table);

                const auto Entry1 = static_cast<PCKTEB>(First);
                const auto Entry2 = static_cast<PCKTEB>(Second);

                return Entry1->ThreadId == Entry2->ThreadId
                    ? GenericEqual
                    : (Entry1->ThreadId > Entry2->ThreadId ? GenericGreaterThan : GenericLessThan);
            }

            PVOID NTAPI Allocate(
                _In_ RTL_AVL_TABLE* Table,
                _In_ ULONG Size
            )
            {
                UNREFERENCED_PARAMETER(Table);

                const auto Buffer = ExAllocateFromLookasideListEx(&MusaCoreTebPool);
                if (Buffer) {
                    RtlSecureZeroMemory(Buffer, Size);
                }
                return Buffer;
            }

            VOID NTAPI Free(
                _In_ RTL_AVL_TABLE* Table,
                _In_ __drv_freesMem(Mem) _Post_invalid_ PVOID Buffer
            )
            {
                UNREFERENCED_PARAMETER(Table);
                return ExFreeToLookasideListEx(&MusaCoreTebPool, Buffer);
            }
        }
    }
}


extern bool MusaCoreUseThreadNotifyCallback;
extern PDRIVER_OBJECT   MusaCoreDriverObject;
extern PCALLBACK_OBJECT MusaCoreThreadNotifyCallbackObject;

static
_IRQL_requires_max_(DISPATCH_LEVEL)
VOID MUSA_NAME_PRIVATE(ThreadNotifyCallbackForTeb)(
    _In_opt_ PVOID  CallbackContext,
    _In_opt_ HANDLE ThreadId,
    _In_opt_ PVOID  Create
)
{
    UNREFERENCED_PARAMETER(CallbackContext);

    if (Create) {
        return;
    }

    PRTL_FLS_DATA FlsData = nullptr;

    const auto LockIrql = MUSA_NAME_PRIVATE(RtlAcquireTebLockExclusive)();
    __try {
        KTEB Block{};
        Block.ThreadId = ThreadId;

        const auto Teb = static_cast<PKTEB>(RtlLookupElementGenericTableAvl(&MusaCoreTebTable, &Block));
        if (Teb) {
            FlsData = Teb->FlsData;
            RtlDeleteElementGenericTableAvl(&MusaCoreTebTable, Teb);
        }

    } __finally {
        MUSA_NAME_PRIVATE(RtlReleaseTebLockExclusive)(LockIrql);
    }

    if (FlsData == nullptr) {
        return;
    }

    if (LockIrql <= APC_LEVEL) {
        RtlProcessFlsData(FlsData,
            RTLP_FLS_DATA_CLEANUP_RUN_CALLBACKS | RTLP_FLS_DATA_CLEANUP_DEALLOCATE);
    }
    else {
        (void)Utils::RunTaskOnLowIrql(MusaCoreDriverObject, [](PVOID FlsData)
        {
            RtlProcessFlsData(FlsData,
                RTLP_FLS_DATA_CLEANUP_RUN_CALLBACKS | RTLP_FLS_DATA_CLEANUP_DEALLOCATE);
            return STATUS_SUCCESS;
        }, FlsData);
    }
}

_Must_inspect_result_
_IRQL_requires_max_(APC_LEVEL)
NTSTATUS MUSA_API MUSA_NAME_PRIVATE(ThreadEnvironmentBlockSetup)(
    _In_ PDRIVER_OBJECT  DriverObject,
    _In_ PUNICODE_STRING RegistryPath
    )
{
    PAGED_CODE();
    UNREFERENCED_PARAMETER(DriverObject);
    UNREFERENCED_PARAMETER(RegistryPath);

    NTSTATUS Status;

    do {
        MusaCoreTebLock = 0;

        constexpr auto BlockSize = ALIGN_DOWN(sizeof(KTEB) + sizeof(RTL_BALANCED_LINKS), sizeof(void*));
        Status = ExInitializeLookasideListEx(&MusaCoreTebPool, nullptr, nullptr,
            NonPagedPool, 0, BlockSize, MUSA_TAG, 0);
        if (!NT_SUCCESS(Status)) {
            break;
        }

        RtlInitializeGenericTableAvl(&MusaCoreTebTable,
            &TebTableRoutines::Compare,
            &TebTableRoutines::Allocate,
            &TebTableRoutines::Free,
            nullptr);

        MusaCoreThreadNotifyCallbackHandleForTeb = ExRegisterCallback(MusaCoreThreadNotifyCallbackObject,
            MUSA_NAME_PRIVATE(ThreadNotifyCallbackForTeb), nullptr);
        if (MusaCoreThreadNotifyCallbackHandleForTeb == nullptr) {
            Status = STATUS_INSUFFICIENT_RESOURCES;
            break;
        }

    } while (false);

    return Status;
}

_Must_inspect_result_
_IRQL_requires_max_(APC_LEVEL)
NTSTATUS MUSA_API MUSA_NAME_PRIVATE(ThreadEnvironmentBlockTeardown)()
{
    PAGED_CODE();

    do {
        if (MusaCoreThreadNotifyCallbackHandleForTeb) {
            ExUnregisterCallback(MusaCoreThreadNotifyCallbackHandleForTeb);
            MusaCoreThreadNotifyCallbackHandleForTeb = nullptr;
        }

        for (auto Entry = RtlGetElementGenericTableAvl(&MusaCoreTebTable, 0);
            Entry;
            Entry = RtlGetElementGenericTableAvl(&MusaCoreTebTable, 0)) {

            RtlDeleteElementGenericTableAvl(&MusaCoreTebTable, Entry);
        }

        if (MusaCoreTebPool.L.Size) {
            ExDeleteLookasideListEx(&MusaCoreTebPool);
        }

    } while (false);

    return STATUS_SUCCESS;
}

_IRQL_requires_max_(DISPATCH_LEVEL)
PVOID MUSA_API MUSA_NAME_PRIVATE(RtlGetCurrentTeb)()
{
    PKTEB   Teb      = nullptr;
    BOOLEAN Expired  = FALSE;

    KTEB  Block{};
    Block.ThreadId  = PsGetCurrentThreadId();
    Block.ProcessId = PsGetCurrentProcessId();
    Block.ProcessEnvironmentBlock = static_cast<PKPEB>(MUSA_NAME_PRIVATE(RtlGetCurrentPeb)());

    // Lookup already exists

    KIRQL LockIrql = MUSA_NAME_PRIVATE(RtlAcquireTebLockShared)();
    __try {
        Teb = static_cast<PKTEB>(RtlLookupElementGenericTableAvl(&MusaCoreTebTable, &Block));
        if (Teb) {
            if (!MusaCoreUseThreadNotifyCallback) {
                if (Teb->ProcessId != Block.ProcessId) {
                    Expired = true;
                    __leave;
                }
            }
            return Teb;
        }
    }
    __finally {
        MUSA_NAME_PRIVATE(RtlReleaseTebLockShared)(LockIrql);
    }

    if (!MusaCoreUseThreadNotifyCallback) {
        if (Expired) {
            ExNotifyCallback(MusaCoreThreadNotifyCallbackHandleForTeb,
                Block.ThreadId, nullptr /* false */);
        }
    }

    // Lazy-init - Insert new item
    LockIrql = MUSA_NAME_PRIVATE(RtlAcquireTebLockExclusive)();
    __try {
        Teb = static_cast<PKTEB>(RtlInsertElementGenericTableAvl(
            &MusaCoreTebTable, &Block, sizeof(KTEB), nullptr));
    }
    __finally {
        MUSA_NAME_PRIVATE(RtlReleaseTebLockExclusive)(LockIrql);
    }

    return Teb;
}

_IRQL_saves_
_IRQL_raises_(DISPATCH_LEVEL)
KIRQL MUSA_API MUSA_NAME_PRIVATE(RtlAcquireTebLockExclusive)()
{
    return ExAcquireSpinLockExclusive(&MusaCoreTebLock);
}

_IRQL_requires_(DISPATCH_LEVEL)
VOID  MUSA_API MUSA_NAME_PRIVATE(RtlReleaseTebLockExclusive)(_In_ _IRQL_restores_ KIRQL Irql)
{
    return ExReleaseSpinLockExclusive(&MusaCoreTebLock, Irql);
}

_IRQL_saves_
_IRQL_raises_(DISPATCH_LEVEL)
KIRQL MUSA_API MUSA_NAME_PRIVATE(RtlAcquireTebLockShared)()
{
    return ExAcquireSpinLockShared(&MusaCoreTebLock);
}

_IRQL_requires_(DISPATCH_LEVEL)
VOID  MUSA_API MUSA_NAME_PRIVATE(RtlReleaseTebLockShared)(_In_ _IRQL_restores_ KIRQL Irql)
{
    return ExReleaseSpinLockShared(&MusaCoreTebLock, Irql);
}

#endif // defined(_KERNEL_MODE)

EXTERN_C_END
