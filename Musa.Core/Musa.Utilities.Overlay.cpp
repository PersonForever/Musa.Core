﻿#include "Musa.Utilities.h"

#if defined(_KERNEL_MODE)

namespace Musa::Utils
{
    NTSTATUS MUSA_API RunTaskOnLowIrql(
        _In_ PDRIVER_OBJECT  DriverObject,
        _In_ PCTASK_FUNCTION Task,
        _In_opt_ PVOID       Context
    )
    {
        VEIL_DECLARE_STRUCT(TASK_ITEM)
        {
            KEVENT   WaitEvent;
            NTSTATUS Status;

            PCTASK_FUNCTION Task;
            PVOID           Context;

            UINT8 IoWorkItem[ANYSIZE_ARRAY];
        };

        NTSTATUS Status;

        do {
            if (DriverObject) {
                const auto TaskItem = static_cast<PTASK_ITEM>(ExAllocatePoolZero(
                    NonPagedPool, sizeof(TASK_ITEM) + IoSizeofWorkItem(), MUSA_TAG));
                if (TaskItem == nullptr) {
                    Status = STATUS_INSUFFICIENT_RESOURCES;
                    break;
                }

                const auto IoWorkItem = reinterpret_cast<PIO_WORKITEM>(TaskItem->IoWorkItem);

                KeInitializeEvent(&TaskItem->WaitEvent, NotificationEvent, FALSE);
                TaskItem->Task    = Task;
                TaskItem->Context = Context;

                IoInitializeWorkItem(DriverObject, IoWorkItem);

                IoQueueWorkItemEx(IoWorkItem, [](PVOID IoObject, PVOID Context, PIO_WORKITEM IoWorkItem)
                {
                    UNREFERENCED_PARAMETER(IoObject);
                    UNREFERENCED_PARAMETER(IoWorkItem);

                    const auto Item = static_cast<PTASK_ITEM>(Context);
                    if (Item) {
                        Item->Status = Item->Task(Item->Context);

                        KeSetEvent(&Item->WaitEvent, IO_NO_INCREMENT, FALSE);
                    }
                }, DelayedWorkQueue, TaskItem);

                Status = KeWaitForSingleObject(&TaskItem->WaitEvent, Executive, KernelMode, FALSE, nullptr);
                if (Status == STATUS_SUCCESS) {
                    Status = TaskItem->Status;
                }

                ExFreePoolWithTag(TaskItem, MUSA_TAG);
            } else {
                #pragma warning(push)
                #pragma warning(disable: 4996)
                const auto TaskItem = static_cast<PTASK_ITEM>(ExAllocatePoolZero(
                    NonPagedPool, sizeof(TASK_ITEM) + sizeof(WORK_QUEUE_ITEM), MUSA_TAG));
                if (TaskItem == nullptr) {
                    Status = STATUS_INSUFFICIENT_RESOURCES;
                    break;
                }

                const auto IoWorkItem = reinterpret_cast<PWORK_QUEUE_ITEM>(TaskItem->IoWorkItem);

                KeInitializeEvent(&TaskItem->WaitEvent, NotificationEvent, FALSE);
                TaskItem->Task    = Task;
                TaskItem->Context = Context;

                ExInitializeWorkItem(IoWorkItem, [](PVOID Context)
                {
                    const auto Item = static_cast<PTASK_ITEM>(Context);
                    if (Item) {
                        Item->Status = Item->Task(Item->Context);

                        KeSetEvent(&Item->WaitEvent, IO_NO_INCREMENT, FALSE);
                    }
                }, TaskItem);

                ExQueueWorkItem(IoWorkItem, DelayedWorkQueue);

                Status = KeWaitForSingleObject(&TaskItem->WaitEvent, Executive, KernelMode, FALSE, nullptr);
                if (Status == STATUS_SUCCESS) {
                    Status = TaskItem->Status;
                }

                ExFreePoolWithTag(TaskItem, MUSA_TAG);
                #pragma warning(pop)
            }
        } while (false);

        return Status;
    }
}

#endif // defined(_KERNEL_MODE)
