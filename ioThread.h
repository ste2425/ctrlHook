#pragma once

#include <pspctrl.h>
#include "systemctrl.h"
#include "pspuart.h"
#include "constants.h"

extern unsigned char xAxis;
extern unsigned char yAxis;
extern void waitForKernel(void);

int ioThread(SceSize args, void *argp);