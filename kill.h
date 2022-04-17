#ifndef KILL_H
#define KILL_H
#include <ntifs.h>

BOOLEAN KillProcess(LONG pid);
BOOLEAN MemKillProcess(LONG pid);

#endif // !KILL_H