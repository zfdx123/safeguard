#include "kill.h"

// ɱ�����̺���
BOOLEAN KillProcess(LONG pid)
{
	HANDLE ProcessHandle;
	NTSTATUS status;
	OBJECT_ATTRIBUTES ObjectAttributes;
	CLIENT_ID Cid;

	// ��ʼ��ObjectAttributes��Cid
	InitializeObjectAttributes(&ObjectAttributes, 0, 0, 0, 0);
	Cid.UniqueProcess = (HANDLE)pid;
	Cid.UniqueThread = 0;
	// �򿪽��̾��
	status = ZwOpenProcess(&ProcessHandle, PROCESS_ALL_ACCESS, &ObjectAttributes, &Cid);
	if (NT_SUCCESS(status))
	{
		DbgPrint("Open Process %d Successful!\n", pid);
		// ��������
		ZwTerminateProcess(ProcessHandle, status);
		// �رվ��
		ZwClose(ProcessHandle);
		return TRUE;
	}
	DbgPrint("Open Process %d Failed!\n", pid);
	return FALSE;
}

BOOLEAN MemKillProcess(LONG pid)
{
	PEPROCESS proc = NULL;
	NTSTATUS ntStatus = STATUS_UNSUCCESSFUL;
	PKAPC_STATE pApcState = NULL;


	PsLookupProcessByProcessId((HANDLE)pid, &proc);
	if (proc == 0)
	{
		return TRUE;
	}

	pApcState = (PKAPC_STATE)ExAllocatePoolWithTag(NonPagedPool, sizeof(PKAPC_STATE), '1111');
	//pApcState = (PKAPC_STATE)ExAllocatePool2(NonPagedPool, sizeof(PKAPC_STATE), '1111');
	if (NULL == pApcState)
	{
		ObDereferenceObject(proc);
		return TRUE;
	}
	__try {
		KeStackAttachProcess((PRKPROCESS)proc, pApcState);
		//KeAttachProcess(proc);
		for (int i = 0x10000; i < 0x20000000; i += PAGE_SIZE)
		{
			__try
			{
				memset((PVOID)i, 0, PAGE_SIZE);
			}
			__except (1)
			{
				;       //�ڲ������쳣
			}
		}
		KeUnstackDetachProcess(pApcState);
		//KeDetachProcess();
		ObDereferenceObject(proc);
		return TRUE;
	}
	__except (1)
	{
		DbgPrint("ǿɱ����\r\n");
		KeUnstackDetachProcess(pApcState);
		ObDereferenceObject(proc);
		return FALSE;
	}

	return TRUE;
}