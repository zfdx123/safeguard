#include <ntifs.h>

NTKERNELAPI NTSTATUS PsSuspendProcess(PEPROCESS proc);	//暂停进程
NTKERNELAPI NTSTATUS PsResumeProcess(PEPROCESS proc);	//恢复进程

PEPROCESS GetEprocessByPid(HANDLE pid)
{
	//根据PID 返回PEPROCESS
	PEPROCESS pEpro = NULL;
	NTSTATUS ntStatus = STATUS_UNSUCCESSFUL;
	ntStatus = PsLookupProcessByProcessId(pid, &pEpro);
	if (NT_SUCCESS(ntStatus))
	{
		return pEpro;
	}
	return NULL;
}

NTSTATUS TestSusPendProcess(ULONG pid)
{
	NTSTATUS status = STATUS_UNSUCCESSFUL;
	PEPROCESS pCurrentEprocess = NULL;
	pCurrentEprocess = GetEprocessByPid((HANDLE)pid);
	if (pCurrentEprocess != NULL)
	{
		PsSuspendProcess(pCurrentEprocess);
		status = STATUS_SUCCESS;
		DbgPrint("挂起进程成功\r\n");
		ObDereferenceObject(pCurrentEprocess);
	}
	return status;
}


NTSTATUS TestPsResumeProcess(ULONG pid)
{
	NTSTATUS status = STATUS_UNSUCCESSFUL;
	PEPROCESS pCurrentEprocess = NULL;
	pCurrentEprocess = GetEprocessByPid((HANDLE)pid);
	if (pCurrentEprocess != NULL)
	{
		PsResumeProcess(pCurrentEprocess);
		status = STATUS_SUCCESS;
		DbgPrint("继续运行进程成功\r\n");
		ObDereferenceObject(pCurrentEprocess);
	}
	return status;
}