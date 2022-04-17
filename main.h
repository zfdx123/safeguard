#ifndef MAIN_H
#define MAIN_H

#include <ntifs.h>

BOOLEAN KillProcess(LONG pid);
BOOLEAN MemKillProcess(LONG pid);
NTSTATUS DeviceApi(PDEVICE_OBJECT Device, PIRP pIrp);
NTSTATUS Unload(IN PDRIVER_OBJECT pDevice);
NTSTATUS DeviceIoctl(IN PDEVICE_OBJECT pDevice, IN PIRP pIrp);
NTSTATUS DriverEntry(IN PDRIVER_OBJECT pDevice, IN PUNICODE_STRING pRegPath);


#endif // !MAIN_H