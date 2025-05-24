EXTERN_C_START

#if defined(_KERNEL_MODE)
LARGE_INTEGER NTAPI MUSA_NAME(RtlGetSystemTimePrecise)(VOID)
{
    LARGE_INTEGER SystemTime{};
    KeQuerySystemTimePrecise(&SystemTime);

    return SystemTime;
}

MUSA_IAT_SYMBOL(RtlGetSystemTimePrecise, 0);
#endif // defined(_KERNEL_MODE)

BOOLEAN NTAPI MUSA_NAME(RtlGetInterruptTime)(
    _Out_ PLARGE_INTEGER InterruptTime
)
{
    #if defined(_KERNEL_MODE)
    InterruptTime->QuadPart = (LONGLONG)KeQueryInterruptTime();
    return TRUE;
    #else // defined(_KERNEL_MODE)
    InterruptTime->QuadPart = *reinterpret_cast<const volatile ULONG64*>(&SharedUserData->InterruptTime);
    return TRUE;
    #endif // !defined(_KERNEL_MODE)
}

MUSA_IAT_SYMBOL(RtlGetInterruptTime, 4);

#if defined(_KERNEL_MODE)
LARGE_INTEGER /* InterruptTime */ NTAPI MUSA_NAME(RtlGetInterruptTimePrecise)(
    _Out_ PLARGE_INTEGER PerformanceCount
)
{
    LARGE_INTEGER InterruptTime;
    InterruptTime.QuadPart = (LONGLONG)KeQueryInterruptTimePrecise(reinterpret_cast<PULONG64>(PerformanceCount));

    return InterruptTime;
}

MUSA_IAT_SYMBOL(RtlGetInterruptTimePrecise, 4);

BOOLEAN NTAPI MUSA_NAME(RtlQueryUnbiasedInterruptTime)(
    _Out_ PLARGE_INTEGER InterruptTime
)
{
    InterruptTime->QuadPart = (LONGLONG)KeQueryUnbiasedInterruptTime();
    return TRUE;
}

MUSA_IAT_SYMBOL(RtlQueryUnbiasedInterruptTime, 4);
#endif // defined(_KERNEL_MODE)

#if (NTDDI_VERSION >= NTDDI_WIN10_RS1)
LARGE_INTEGER NTAPI MUSA_NAME(RtlQueryUnbiasedInterruptTimePrecise)(
    _Out_ PLARGE_INTEGER PerformanceCount
)
{
    #if defined(_KERNEL_MODE)
    LARGE_INTEGER InterruptTime;
    InterruptTime.QuadPart = (LONGLONG)KeQueryUnbiasedInterruptTimePrecise(
        reinterpret_cast<PULONG64>(PerformanceCount));

    return InterruptTime;
    #else // defined(_KERNEL_MODE)
    ULONGLONG InterruptTimeBias;
    ULONGLONG InterruptTimePrecise;

    do
    {
        InterruptTimeBias    = SharedUserData->InterruptTimeBias;
        InterruptTimePrecise = RtlGetInterruptTimePrecise(PerformanceCount).QuadPart;

    } while (InterruptTimeBias != SharedUserData->InterruptTimeBias);

    LARGE_INTEGER Result;
    Result.QuadPart = InterruptTimePrecise - InterruptTimeBias;

    return Result;
    #endif // !defined(_KERNEL_MODE)
}

MUSA_IAT_SYMBOL(RtlQueryUnbiasedInterruptTimePrecise, 4);
#endif // (NTDDI_VERSION >= NTDDI_WIN10_RS1)

NTSTATUS NTAPI MUSA_NAME(RtlQueryAuxiliaryCounterFrequency)(
    _Out_ PLARGE_INTEGER AuxiliaryCounterFrequency
)
{
    #if defined(_KERNEL_MODE)
    return KeQueryAuxiliaryCounterFrequency(reinterpret_cast<PULONG64>(AuxiliaryCounterFrequency));
    #else
    return ZwQueryAuxiliaryCounterFrequency(AuxiliaryCounterFrequency);
    #endif
}

MUSA_IAT_SYMBOL(RtlQueryAuxiliaryCounterFrequency, 4);

NTSTATUS NTAPI MUSA_NAME(RtlConvertAuxiliaryCounterToPerformanceCounter)(
    _In_ ULONG64             AuxiliaryCounterValue,
    _Out_ PLARGE_INTEGER     PerformanceCounterValue,
    _Out_opt_ PLARGE_INTEGER ConversionError
)
{
    #if defined(_KERNEL_MODE)
    return KeConvertAuxiliaryCounterToPerformanceCounter(AuxiliaryCounterValue,
        reinterpret_cast<PULONG64>(PerformanceCounterValue), reinterpret_cast<PULONG64>(ConversionError));
    #else
    LARGE_INTEGER Value;
    Value.QuadPart = AuxiliaryCounterValue;

    return ZwConvertBetweenAuxiliaryCounterAndPerformanceCounter(
        TRUE, &Value, PerformanceCounterValue, ConversionError);

    #endif
}

MUSA_IAT_SYMBOL(RtlConvertAuxiliaryCounterToPerformanceCounter, 16);

NTSTATUS NTAPI MUSA_NAME(RtlConvertPerformanceCounterToAuxiliaryCounter)(
    _In_ ULONG64             PerformanceCounterValue,
    _Out_ PLARGE_INTEGER     AuxiliaryCounterValue,
    _Out_opt_ PLARGE_INTEGER ConversionError
)
{
    #if defined(_KERNEL_MODE)
    return KeConvertPerformanceCounterToAuxiliaryCounter(PerformanceCounterValue,
        reinterpret_cast<PULONG64>(AuxiliaryCounterValue), reinterpret_cast<PULONG64>(ConversionError));
    #else
    LARGE_INTEGER Value;
    Value.QuadPart = PerformanceCounterValue;

    return ZwConvertBetweenAuxiliaryCounterAndPerformanceCounter(
        FALSE, &Value, AuxiliaryCounterValue, ConversionError);
    #endif
}

MUSA_IAT_SYMBOL(RtlConvertPerformanceCounterToAuxiliaryCounter, 16);

ULONG NTAPI MUSA_NAME(RtlGetTickCount)(VOID)
{
    return ZwGetTickCount();
}

MUSA_IAT_SYMBOL(RtlGetTickCount, 0);

LARGE_INTEGER NTAPI MUSA_NAME(RtlGetTickCount64)(VOID)
{
    return {.QuadPart = (LONGLONG)ZwGetTickCount64()};
}

MUSA_IAT_SYMBOL(RtlGetTickCount64, 0);

EXTERN_C_END
