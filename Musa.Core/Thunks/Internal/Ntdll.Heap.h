#pragma once

EXTERN_C_START

_IRQL_requires_max_(APC_LEVEL)
_Must_inspect_result_
PVOID NTAPI MUSA_NAME(RtlCreateHeap)(
    _In_ ULONG      Flags,
    _In_opt_ PVOID  HeapBase,
    _In_opt_ SIZE_T ReserveSize,
    _In_opt_ SIZE_T CommitSize,
    _In_opt_ PVOID  Lock,
    _When_((Flags & 0x100) != 0,
        _In_reads_bytes_opt_(sizeof(RTL_SEGMENT_HEAP_PARAMETERS)))
    _When_((Flags & 0x100) == 0,
        _In_reads_bytes_opt_(sizeof(RTL_HEAP_PARAMETERS)))
    PRTL_HEAP_PARAMETERS Parameters
    );

_IRQL_requires_max_(APC_LEVEL)
PVOID NTAPI MUSA_NAME(RtlDestroyHeap)(
    _In_ _Post_invalid_ PVOID HeapHandle
    );

_IRQL_requires_max_(APC_LEVEL)
ULONG NTAPI MUSA_NAME(RtlGetProcessHeaps)(
    _In_ ULONG   NumberOfHeaps,
    _Out_ PVOID* ProcessHeaps
    );

_IRQL_requires_max_(APC_LEVEL)
NTSTATUS NTAPI MUSA_NAME(RtlEnumProcessHeaps)(
    _In_ PRTL_ENUM_HEAPS_ROUTINE EnumRoutine,
    _In_ PVOID                   Parameter
    );

EXTERN_C_END
