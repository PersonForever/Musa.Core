﻿#pragma once

namespace Musa
{
    #define BaseSetLastNTError RtlSetLastWin32ErrorAndNtStatusFromNtStatus

    PVOID WINAPI BaseIsRealtimeAllowed(
        BOOLEAN LeaveEnabled,
        BOOLEAN Revert
        );

}
