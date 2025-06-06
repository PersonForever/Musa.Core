﻿#pragma once
#pragma warning(disable: 6101 28101 28167)

// Config Macro
#define POOL_NX_OPTIN 1
#define POOL_ZERO_DOWN_LEVEL_SUPPORT 1
#define RTL_USE_AVL_TABLES

// unnecessary, fix ReSharper's code analysis.
#pragma warning(suppress: 4117)
#define _KERNEL_MODE 1

// System Header
#include <Veil.h>

// C/C++  Header
#include <stddef.h>
#include <stdlib.h>
#include <string.h>

// Local  Header
#include <Musa.CoreLite.h>
#include "Musa.Core/Musa.Core.h"

// Global Variable
constexpr unsigned long MUSA_TAG = '-iM-';

// Global Macro
#define MUSA_NAME_PUBLIC(name)  _VEIL_CONCATENATE(_Musa_, name)
#define MUSA_NAME_PRIVATE(name) _VEIL_CONCATENATE(_Musa_Private_, name)
#define MUSA_NAME MUSA_NAME_PUBLIC

#if defined _M_IX86
#define MUSA_IAT_SYMBOL(name, stack) _VEIL_DEFINE_IAT_RAW_SYMBOL(_VEIL_CONCATENATE(_VEIL_CONCATENATE(name,@),stack), MUSA_NAME(name))
#else
#define MUSA_IAT_SYMBOL(name, stack) _VEIL_DEFINE_IAT_SYMBOL(name, MUSA_NAME(name))
#endif

#define MUSA_ALTERNATE_NAME(name) _VEIL_DECLARE_ALTERNATE_NAME(name, MUSA_NAME(name))

// Logging
#ifdef _DEBUG
#define MusaLOG(fmt, ...) DbgPrintEx(DPFLTR_DEFAULT_ID, DPFLTR_ERROR_LEVEL, \
    "[Musa.Core][%s():%u]" fmt "\n", __FUNCTION__, __LINE__, ## __VA_ARGS__)
#else
#define MusaLOG(...)
#endif
