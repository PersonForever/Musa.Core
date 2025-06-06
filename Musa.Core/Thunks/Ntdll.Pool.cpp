﻿EXTERN_C_START

#if defined(_KERNEL_MODE)

NTSTATUS NTAPI MUSA_NAME(RtlCreateMemoryBlockLookaside)(
    _Out_ PVOID*     MemoryBlockLookaside,
    _Reserved_ ULONG Flags, // POOL_FLAG_
    _In_ ULONG       InitialSize,
    _In_ ULONG       MinimumBlockSize,
    _In_ ULONG       MaximumBlockSize
)
{
    UNREFERENCED_PARAMETER(InitialSize);

    if (MinimumBlockSize > MaximumBlockSize) {
        return STATUS_INVALID_PARAMETER;
    }

    if (Flags == 0) {
        Flags = POOL_FLAG_PAGED;
    }

    POOL_TYPE PoolType;

    switch (Flags) {
        case POOL_FLAG_PAGED:
            PoolType = PagedPool;
            break;
        case POOL_FLAG_NON_PAGED:
            PoolType = NonPagedPoolNx;
            break;
        case POOL_FLAG_NON_PAGED_EXECUTE:
            PoolType = NonPagedPool;
            break;
        default:
            return STATUS_INVALID_PARAMETER;
    }

    *MemoryBlockLookaside = ExAllocatePoolZero(NonPagedPoolNx,
        sizeof(LOOKASIDE_LIST_EX), MUSA_TAG);
    if (*MemoryBlockLookaside == nullptr) {
        return STATUS_INSUFFICIENT_RESOURCES;
    }

    const auto Status = ExInitializeLookasideListEx(static_cast<PLOOKASIDE_LIST_EX>(*MemoryBlockLookaside),
        nullptr, nullptr, PoolType, 0, MaximumBlockSize, 0, 0);
    if (!NT_SUCCESS(Status)) {
        ExFreePoolWithTag(*MemoryBlockLookaside, MUSA_TAG);

        *MemoryBlockLookaside = nullptr;
    }

    return Status;
}

MUSA_IAT_SYMBOL(RtlCreateMemoryBlockLookaside, 20);

NTSTATUS NTAPI MUSA_NAME(RtlDestroyMemoryBlockLookaside)(
    _In_ PVOID MemoryBlockLookaside
)
{
    if (MemoryBlockLookaside) {
        ExDeleteLookasideListEx(
            static_cast<PLOOKASIDE_LIST_EX>(MemoryBlockLookaside));

        ExFreePoolWithTag(MemoryBlockLookaside, MUSA_TAG);
    }

    return STATUS_SUCCESS;
}

MUSA_IAT_SYMBOL(RtlDestroyMemoryBlockLookaside, 4);

NTSTATUS NTAPI MUSA_NAME(RtlAllocateMemoryBlockLookaside)(
    _In_ PVOID   MemoryBlockLookaside,
    _In_ ULONG   BlockSize,
    _Out_ PVOID* Block
)
{
    const auto Lookaside = static_cast<PLOOKASIDE_LIST_EX>(MemoryBlockLookaside);
    if (BlockSize > Lookaside->L.Size) {
        return STATUS_INVALID_PARAMETER;
    }

    *Block = ExAllocateFromLookasideListEx(
        static_cast<PLOOKASIDE_LIST_EX>(MemoryBlockLookaside));
    if (*Block == nullptr) {
        return STATUS_INSUFFICIENT_RESOURCES;
    }

    return STATUS_SUCCESS;
}

MUSA_IAT_SYMBOL(RtlAllocateMemoryBlockLookaside, 12);

NTSTATUS NTAPI MUSA_NAME(RtlFreeMemoryBlockLookaside)(
    _In_ PVOID MemoryBlockLookaside,
    _In_ PVOID Block
)
{
    if (Block) {
        ExFreeToLookasideListEx(
            static_cast<PLOOKASIDE_LIST_EX>(MemoryBlockLookaside), Block);
    }
    return STATUS_SUCCESS;
}

MUSA_IAT_SYMBOL(RtlFreeMemoryBlockLookaside, 8);

NTSTATUS NTAPI MUSA_NAME(RtlExtendMemoryBlockLookaside)(
    _In_ PVOID MemoryBlockLookaside,
    _In_ ULONG Increment
)
{
    UNREFERENCED_PARAMETER(MemoryBlockLookaside);
    UNREFERENCED_PARAMETER(Increment);
    return STATUS_NOT_SUPPORTED;
}

MUSA_IAT_SYMBOL(RtlExtendMemoryBlockLookaside, 8);

NTSTATUS NTAPI MUSA_NAME(RtlResetMemoryBlockLookaside)(
    _In_ PVOID MemoryBlockLookaside
)
{
    UNREFERENCED_PARAMETER(MemoryBlockLookaside);
    return STATUS_NOT_SUPPORTED;
}

MUSA_IAT_SYMBOL(RtlResetMemoryBlockLookaside, 4);

NTSTATUS NTAPI MUSA_NAME(RtlLockMemoryBlockLookaside)(
    _In_ PVOID MemoryBlockLookaside
)
{
    UNREFERENCED_PARAMETER(MemoryBlockLookaside);
    return STATUS_NOT_SUPPORTED;
}

MUSA_IAT_SYMBOL(RtlLockMemoryBlockLookaside, 4);

NTSTATUS NTAPI MUSA_NAME(RtlUnlockMemoryBlockLookaside)(
    _In_ PVOID MemoryBlockLookaside
)
{
    UNREFERENCED_PARAMETER(MemoryBlockLookaside);
    return STATUS_NOT_SUPPORTED;
}

MUSA_IAT_SYMBOL(RtlUnlockMemoryBlockLookaside, 4);

#endif // defined(_KERNEL_MODE)

EXTERN_C_END
