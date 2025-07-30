#pragma once

#include <pspctrl.h>
#include "systemctrl.h"

extern unsigned char xAxis;
extern unsigned char yAxis;
extern void waitForKernel(void);

int controllerPatchingThread(SceSize args, void *argp);