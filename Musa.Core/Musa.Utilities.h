﻿#pragma once

EXTERN_C uintptr_t __security_cookie;

namespace Musa::Utils
{
    // type traits
    template <class, class>
    inline constexpr bool is_same_v = false; // determine whether arguments are the same type
    template <class T>
    inline constexpr bool is_same_v<T, T> = true;

    // FastDecodePointer & FastEncodePointer
    inline uintptr_t RotatePointerValue(uintptr_t Value, int const Shift) noexcept
    {
#if defined(_WIN64)
        return RotateRight64(Value, Shift);
#else
        return RotateRight32(Value, Shift);
#endif
    }

    constexpr int MAXIMUM_POINTER_SHIFT = sizeof(uintptr_t) * 8;

    template <typename T>
    constexpr T FastDecodePointer(T const Ptr, uintptr_t Cookie = __security_cookie) noexcept
    {
        return reinterpret_cast<T>(RotatePointerValue(reinterpret_cast<uintptr_t>(Ptr),
            MAXIMUM_POINTER_SHIFT - static_cast<int>(Cookie % MAXIMUM_POINTER_SHIFT)) ^ Cookie);
    }

    template <typename T>
    constexpr T FastEncodePointer(T const Ptr, uintptr_t Cookie = __security_cookie) noexcept
    {
        return reinterpret_cast<T>(RotatePointerValue(reinterpret_cast<uintptr_t>(Ptr) ^ Cookie,
            static_cast<int>(Cookie % MAXIMUM_POINTER_SHIFT)));
    }

    // Hash
    constexpr size_t Fnv1aHash(_In_ const char* Buffer, _In_ const size_t Count) noexcept
    {
#if defined(_WIN64)
        constexpr size_t FnvOffsetBasis = 14695981039346656037ULL;
        constexpr size_t FnvPrime = 1099511628211ULL;
#else
        constexpr size_t FnvOffsetBasis = 2166136261U;
        constexpr size_t FnvPrime = 16777619U;
#endif

        auto Value = FnvOffsetBasis;
        for (size_t idx = 0; idx < Count; ++idx) {
            Value ^= static_cast<size_t>(Buffer[idx]);
            Value *= FnvPrime;
        }
        return Value;
    }

    template<size_t Size>
    constexpr size_t Fnv1aHash(const char(&Buffer)[Size]) noexcept
    {
        return Fnv1aHash(Buffer, Size - _countof(""));
    }

    NTSTATUS MUSA_API GetLoadedModuleBase(
        _Out_ PVOID* ModuleBase,
        _In_  PCWSTR ModuleName
    );

    NTSTATUS MUSA_API GetKnownDllSectionHandle(
        _Out_ HANDLE* SectionHandle,
        _In_  PCWSTR  DllName,
        _In_  ACCESS_MASK DesiredAccess
    );

    NTSTATUS MUSA_API RemapSectionView(
        _Inout_ HANDLE* SectionHandle,
        _In_    PVOID* ViewBase,
        _In_    SIZE_T* ViewSize
    );

#ifdef _KERNEL_MODE
    typedef
        _IRQL_requires_max_(APC_LEVEL)
        _Function_class_(TASK_FUNCTION)
        NTSTATUS
        CALLBACK
        TASK_FUNCTION(
            _In_opt_ PVOID Context
        );
    typedef TASK_FUNCTION* PCTASK_FUNCTION;

    NTSTATUS MUSA_API RunTaskOnLowIrql(
        _In_ PDRIVER_OBJECT  DriverObject,
        _In_ PCTASK_FUNCTION Task,
        _In_opt_ PVOID Context
    );
#endif

}
