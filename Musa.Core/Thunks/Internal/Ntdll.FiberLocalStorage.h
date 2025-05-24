#pragma once

EXTERN_C_START

_IRQL_requires_max_(APC_LEVEL)
VOID NTAPI MUSA_NAME(RtlProcessFlsData)(
    _In_ PVOID FlsData,
    _In_ ULONG Flags
    );

EXTERN_C_END
