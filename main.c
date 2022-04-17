#include "main.h"
#include "kill.h"
#include "protection.h"
#include "pendprocess.h"

//杀死进程
#define IOCTL_KILL CTL_CODE(FILE_DEVICE_UNKNOWN, 0x800, METHOD_BUFFERED, FILE_ANY_ACCESS)
//保护进程
#define IOCTL_PRESS CTL_CODE(FILE_DEVICE_UNKNOWN, 0x801, METHOD_BUFFERED, FILE_ANY_ACCESS)
//取消保护
#define IOCTL_PRESS_2 CTL_CODE(FILE_DEVICE_UNKNOWN, 0x802, METHOD_BUFFERED, FILE_ANY_ACCESS)
//暂停进程
#define IOCTL_PUASE CTL_CODE(FILE_DEVICE_UNKNOWN, 0x803, METHOD_BUFFERED, FILE_ANY_ACCESS)
//恢复进程
#define IOCTL_PUASE_2 CTL_CODE(FILE_DEVICE_UNKNOWN, 0x804,METHOD_BUFFERED, FILE_ANY_ACCESS)


#define DEVICE_OBJECT_NAME  L"\\Device\\Safe"
//设备与设备之间通信
#define DEVICE_LINK_NAME    L"\\??\\Safe"
//设备与Ring3之间通信

UNICODE_STRING ustrDevName;
UNICODE_STRING ustrSymName;
PDEVICE_OBJECT paDevice;

NTSTATUS DeviceApi(PDEVICE_OBJECT Device, PIRP pIrp)
{
	pIrp->IoStatus.Status = STATUS_SUCCESS;
	pIrp->IoStatus.Information = 0;
	IoCompleteRequest(pIrp, IO_NO_INCREMENT);
	return STATUS_SUCCESS;
}

NTSTATUS Unload(IN PDRIVER_OBJECT pDevice) {
	UnHook();
	IoDeleteSymbolicLink(&ustrSymName);
	IoDeleteDevice(pDevice->DeviceObject);
	DbgPrint(("Unload\n") );
	return STATUS_SUCCESS;
}


NTSTATUS DeviceIoctl(IN PDEVICE_OBJECT pDevice, IN PIRP pIrp) {
	NTSTATUS status;
	PIO_STACK_LOCATION irps = IoGetCurrentIrpStackLocation(pIrp);
	ULONG CODE = irps->Parameters.DeviceIoControl.IoControlCode;
	ULONG info = 0;
	switch (CODE)
	{
	case IOCTL_KILL: {
		LONG pid = *(PLONG)(pIrp->AssociatedIrp.SystemBuffer);
		if (KillProcess(pid))
		{
			DbgPrint("Kill Successful\n");
		}
		else
		{
			DbgPrint("Kill Failed\n");
			DbgPrint("Start MemKillProcess\n");
			if (MemKillProcess(pid)) {
				DbgPrint("Kill Successful\n");
			}
		}
		status = STATUS_SUCCESS;
		break;
	}
	case IOCTL_PRESS: {          
		LONG pid = *(PLONG)(pIrp->AssociatedIrp.SystemBuffer);
		DbgPrint("保护PID %d\n",pid);
		Variable(pid);
		InstallHook();
		status = STATUS_SUCCESS;
		break;
	}
	case IOCTL_PRESS_2: {
		UnHook();
		status = STATUS_SUCCESS;
		break;
	}
	case IOCTL_PUASE: {
		LONG pid = *(PLONG)(pIrp->AssociatedIrp.SystemBuffer);
		DbgPrint("暂停PID进程 %d\n", pid);
		status = TestSusPendProcess(pid);
		break;
	}
	case IOCTL_PUASE_2: {
		LONG pid = *(PLONG)(pIrp->AssociatedIrp.SystemBuffer);
		DbgPrint("继续PID进程 %d\n", pid);
		status = TestPsResumeProcess(pid);
		break;
	}

	default:
		DbgPrint(("Unkown code!\n"));
		status = STATUS_UNSUCCESSFUL;
		break;
	}
	pIrp->IoStatus.Status = status;
	pIrp->IoStatus.Information = info;
	IoCompleteRequest(pIrp, IO_NO_INCREMENT);
	return status;
}

NTSTATUS DriverEntry(IN PDRIVER_OBJECT pDevice, IN PUNICODE_STRING pRegPath) {
	NTSTATUS status;

	pDevice->DriverUnload = Unload;

	for (int i = 0; i < IRP_MJ_MAXIMUM_FUNCTION; i++) {
		pDevice->MajorFunction[i] = DeviceApi;
	}

	pDevice->MajorFunction[IRP_MJ_DEVICE_CONTROL] = DeviceIoctl;

	RtlInitUnicodeString(&ustrDevName, DEVICE_OBJECT_NAME);
	RtlInitUnicodeString(&ustrSymName, DEVICE_LINK_NAME);

	DbgPrint("Start");

	status = IoCreateDevice(pDevice, sizeof(pDevice->DriverExtension), &ustrDevName, FILE_DEVICE_UNKNOWN, 0, TRUE, &paDevice);
	if (!NT_SUCCESS(status)) {
		DbgPrint("IoCreateDevice %08X\r\n", status);
		return status;
	}

	// 创建符号链接, 并与设备相关联
	status = IoCreateSymbolicLink(&ustrSymName, &ustrDevName);
	if (!NT_SUCCESS(status))
	{
		DbgPrint("IoCreateSymbolicLink %08X\r\n", status);
		IoDeleteDevice(paDevice);
		return status;
	}

	//PLDR_DATA ldr;
	//ldr = (PLDR_DATA)pDevice->DriverSection;
	//ldr->Flags |= 0x20;//加载驱动的时候会判断此值。必须有特殊签名才行，增加0x20即可。否则将调用失败   

	//GetProcessNameByProcessId((HANDLE)pid);

	paDevice->Flags |= DO_BUFFERED_IO;

	return STATUS_SUCCESS;

}