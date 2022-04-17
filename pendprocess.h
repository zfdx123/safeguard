#ifndef PENDPROCESS_H
#define PENDPROCESS_H
#include <ntifs.h>

NTSTATUS TestSusPendProcess(ULONG pid);
NTSTATUS TestPsResumeProcess(ULONG pid);

#endif // !PENDPROCESS_H
