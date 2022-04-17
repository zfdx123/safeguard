#include <ntifs.h>

NTKERNELAPI NTSTATUS PsSuspendProcess(PEPROCESS proc);	//��ͣ����
NTKERNELAPI NTSTATUS PsResumeProcess(PEPROCESS proc);	//�ָ�����

PEPROCESS GetEprocessByPid(HANDLE pid)
{
	//����PID ����PEPROCESS
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
		DbgPrint("������̳ɹ�\r\n");
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
		DbgPrint("�������н��̳ɹ�\r\n");
		ObDereferenceObject(pCurrentEprocess);
	}
	return status;
}