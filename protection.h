#ifndef PROTECTION_H
#define PROTECTION_H
#include <ntifs.h>

VOID Variable(ULONG pid);
void InstallHook();
VOID UnHook();

#endif // !PROTECTION_H
