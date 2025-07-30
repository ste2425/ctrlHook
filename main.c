//
// OP-DITTO ctrlHook (c) 2021 by Operation Ditto Team
// 
// OP-DITTO ctrlHook is licensed under a
// Creative Commons Attribution-ShareAlike 4.0 International License.
// 
// You should have received a copy of the license along with this
// work. If not, see <http://creativecommons.org/licenses/by-sa/4.0/>.
//
// OP-DITTO ctrlHook is loosely based upon the syscall hook example 
// written by fLaSh, it can be found here:
// https://wololo.net/talk/viewtopic.php?f=5&t=11689
//
#include "constants.h"
#include "defines.h"
#include "controllerPatchingThread.h"
#include "ioThread.h"

PSP_MODULE_INFO(PLUGIN_NAME, 0x1000, 0, 1);
PSP_MAIN_THREAD_ATTR(0); 

// Default right analog to center position
unsigned char xAxis = 127;
unsigned char yAxis = 127;

void waitForKernel()
{
    /* Wait for the kernel to boot */
    while (sceKernelFindModuleByName(PSP_KERNEL_MODULE_NAME) == NULL)
    {        
        sceKernelDelayThread(10000); /* wait 10 milliseconds */
    }
}

void startThread(const char *threadName, SceKernelThreadEntry threadFunc, SceSize args, void *argp)
{
    SceUID thid;

    thid = sceKernelCreateThread(MAIN_THREAD_NAME, threadFunc, 0x18, 0x500, 0, NULL);
    if (thid >= 0) {
        sceKernelStartThread(thid, args, argp);
    }
}

void terminateThread(const char *threadName)
{
    SceUID thid;   
    pspSdkReferThreadStatusByName(threadName, &thid, NULL);
    
    sceKernelTerminateThread(thid);
}

int resumeHandler(int unk, void *param)
{
    SceUID ioThid;

    ioThid = sceKernelCreateThread(IO_THREAD_NAME, ioThread, 0x18, 0x500, 0, NULL);
    if (ioThid >= 0) {
        sceKernelStartThread(ioThid, 0, NULL);
    }

  return 0;
}

// UART seems to reset upon suspend and stops working, so we need to terminate it
// and re-initialize it upon resume.
int suspendHandler(int unk, void *param)
{
    SceUID ioThid;   
    pspSdkReferThreadStatusByName(IO_THREAD_NAME, &ioThid, NULL);
    
    // If we call `sceKernelTerminateThread` (which would sounds logical) it will hang the PSP after
    // the second pause and resume cycle. Just FYI.
    sceKernelTerminateDeleteThread(ioThid);

    pspUARTTerminate();

    return 0;
}

/* Create a user thread */
int module_start(SceSize args, void *argp)
{ 
    startThread(MAIN_THREAD_NAME, controllerPatchingThread, args, argp);
    startThread(IO_THREAD_NAME, ioThread, args, argp);
    
    sceKernelRegisterSuspendHandler(0x1F, suspendHandler, 0);

    sceKernelRegisterResumeHandler(0x1F, resumeHandler, 0);

    return 0;
}

/* terminate the entire module, so we can return to XMB without freezing */
int module_stop(SceSize args, void *argp)
{
    terminateThread(MAIN_THREAD_NAME);
    terminateThread(IO_THREAD_NAME);

    pspUARTTerminate();

    return 0;
}