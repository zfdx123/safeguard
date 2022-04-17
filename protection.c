#include "protection.h"

//OriginalDesiredAccess为原本权限，DesiredAccess为即将要更改的新权限
//如果要结束进程,进程管理器结束进程发送0x1001，taskkill指令结束进程发送0x0001，taskkil加/f参数结束进程发送0x1401 
// 来自GitHub解释
//进程管理器详细界面结束代码
#define PROCESS_TERMINATE_0       0x1001
//taskkill指令结束代码
#define PROCESS_TERMINATE_1       0x0001 
//taskkill指令加/f参数强杀进程结束码
#define PROCESS_KILL_F			  0x1401
//进程管理器结束代码
#define PROCESS_TERMINATE_2       0x1041

//NTKERNELAPI UCHAR* PsGetProcessImageFileName(IN PEPROCESS Process);

HANDLE g_protectpid = NULL;
PVOID g_pRegistrationHandle;
//PUCHAR g_name;

VOID Variable(ULONG pid) 
{
	g_protectpid = (HANDLE)pid;
	//PEPROCESS getID = NULL;
	//PsLookupProcessByProcessId((HANDLE)pid, &getID);
	//PUCHAR pProcessName = PsGetProcessImageFileName(getID);
	//g_name = pProcessName;
}

OB_PREOP_CALLBACK_STATUS precessCallBack(PVOID RegistrationContext, POB_PRE_OPERATION_INFORMATION pOperationInformation)
{
	PEPROCESS pEProcess = NULL;
	// 判断对象类型 
	if (*PsProcessType != pOperationInformation->ObjectType)
	{
		return OB_PREOP_SUCCESS;
	}

	//获取该进程结构对象的名称
	//pEProcess = (PEPROCESS)pOperationInformation->Object;
	//PUCHAR pProcessName = PsGetProcessImageFileName(pEProcess);
	HANDLE pid = PsGetProcessId((PEPROCESS)pOperationInformation->Object);
	// 判断是否为保护进程，不是则放行
	if (NULL != pEProcess)
	{
		//if (!(0 == _stricmp(pProcessName, g_name)))
		if(!(pid == g_protectpid))
		{
			return OB_PREOP_SUCCESS;
		}
	}
	// 判断操作类型,如果该句柄是终止操作，则拒绝该操作
	switch (pOperationInformation->Operation)
	{
	case OB_OPERATION_HANDLE_DUPLICATE:
	{
		break;
	}
	case OB_OPERATION_HANDLE_CREATE:
	{
		//如果要结束进程,进程管理器结束进程发送0x1001，taskkill指令结束进程发送0x0001，taskkil加/f参数结束进程发送0x1401
		//int code = pOperationInformation->Parameters->CreateHandleInformation.OriginalDesiredAccess;
		switch (pOperationInformation->Parameters->CreateHandleInformation.OriginalDesiredAccess)
		{
		case PROCESS_TERMINATE_0: {
			pOperationInformation->Parameters->CreateHandleInformation.DesiredAccess = 0;
			DbgPrint("拒绝执行当前操作 0x%08x\n",PROCESS_TERMINATE_0);
			break;
		}
		case PROCESS_TERMINATE_1: {
			pOperationInformation->Parameters->CreateHandleInformation.DesiredAccess = 0;
			DbgPrint("拒绝执行当前操作 0x%08x\n",PROCESS_TERMINATE_1);
			break;
		}
		case PROCESS_KILL_F: {
			pOperationInformation->Parameters->CreateHandleInformation.DesiredAccess = 0;
			DbgPrint("拒绝执行当前操作 0x%08x\n", PROCESS_KILL_F);
			break;
		}
		case PROCESS_TERMINATE_2: {
			pOperationInformation->Parameters->CreateHandleInformation.DesiredAccess = STANDARD_RIGHTS_ALL;
			DbgPrint("恢复权限 0x%08x\n", PROCESS_TERMINATE_2);
			break;
		}
		/*default: {
			int code = pOperationInformation->Parameters->CreateHandleInformation.OriginalDesiredAccess;
			DbgPrint("未知操作 0x%08x\n",code);
			break;
		}*/
		}
		break;
	}
	default :
		DbgPrint("新东西??\n");
		break;
	}
	return OB_PREOP_SUCCESS;
}

///Post operation callback
VOID ObjectPostCallback(IN PVOID RegistrationContext, IN POB_POST_OPERATION_INFORMATION OperationInformation)
{
	if (PsGetProcessId((PEPROCESS)OperationInformation->Object) == g_protectpid && g_protectpid != 0 && g_protectpid != NULL)
	{
		UNREFERENCED_PARAMETER(RegistrationContext);
		DbgPrintEx(DPFLTR_IHVDRIVER_ID, DPFLTR_INFO_LEVEL, "PostProcCreateRoutine. \n");
	}
}

void InstallHook()
{
	NTSTATUS status;

	OB_CALLBACK_REGISTRATION obReg;
	OB_OPERATION_REGISTRATION obOper;

	DbgPrint("注册保护");
	obOper.ObjectType = PsProcessType;
	obOper.Operations = OB_OPERATION_HANDLE_CREATE | OB_OPERATION_HANDLE_DUPLICATE;
	obOper.PreOperation = (POB_PRE_OPERATION_CALLBACK)&precessCallBack;
	obOper.PostOperation = &ObjectPostCallback;

	RtlInitUnicodeString(&obReg.Altitude, L"600000");
	obReg.Version = ObGetFilterVersion();
	//obReg.Version = OB_FLT_REGISTRATION_VERSION;
	obReg.OperationRegistrationCount = 1;
	obReg.RegistrationContext = NULL;
	obReg.OperationRegistration = &obOper;

	status = ObRegisterCallbacks(&obReg, &g_pRegistrationHandle);
	if (status == STATUS_SUCCESS) {
		DbgPrint("Register Callback Function Successful!\n");
	}
	if (status == STATUS_FLT_INSTANCE_ALTITUDE_COLLISION) {
		DbgPrintEx(DPFLTR_IHVDRIVER_ID, DPFLTR_ERROR_LEVEL, "STATUS_FLT_INSTANCE_ALTITUDE_COLLISION\n");
	}
	if (status == STATUS_INVALID_PARAMETER) {
		DbgPrintEx(DPFLTR_IHVDRIVER_ID, DPFLTR_ERROR_LEVEL, "STATUS_INVALID_PARAMETER\n");
	}
	if (status == STATUS_ACCESS_DENIED) {
		DbgPrintEx(DPFLTR_IHVDRIVER_ID, DPFLTR_ERROR_LEVEL, "STATUS_ACCESS_DENIED\n");
	}
	if (status == STATUS_INSUFFICIENT_RESOURCES) {
		DbgPrintEx(DPFLTR_IHVDRIVER_ID, DPFLTR_ERROR_LEVEL, "STATUS_INSUFFICIENT_RESOURCES\n");
	}
	//DbgPrintEx(DPFLTR_IHVDRIVER_ID, DPFLTR_INFO_LEVEL, "NTSTATUS 0x%08x\n", status);
	DbgPrint("完成注册");
}

VOID UnHook() {
	if (g_pRegistrationHandle != NULL) {
		ObUnRegisterCallbacks(g_pRegistrationHandle);
	}
}