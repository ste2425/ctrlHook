#include "controllerPatchingThread.h"

void *hooked_readbuffer_func;

s32 sceCtrlReadBufferPositive_patch(SceCtrlData *data, u8 nBufs)
{
    int k1 = pspSdkSetK1(0);

    // create a function ptr to sceCtrlReadBufferPositive() to fill the buttons normally
    s32 (*hooked_readbuffer_func)(SceCtrlData*, u8) = (s32 (*)(SceCtrlData*, u8))sctrlHENFindFunction("sceController_Service", "sceCtrl", 0x1F803938);
    hooked_readbuffer_func(data, nBufs);

    data->Rsrv[0] = xAxis;
    data->Rsrv[1] = yAxis;

    pspSdkSetK1(k1);
    return 0;
}

int controllerPatchingThread(SceSize args, void *argp)
{
    // dont do anything until we can confirm all modules are loaded
    waitForKernel();    

    // enable analog sampling in the kernel
    sceCtrlSetSamplingMode(PSP_CTRL_MODE_ANALOG);   

    // grab readbufferpositive from the NID
    hooked_readbuffer_func = (void *)sctrlHENFindFunction("sceController_Service", "sceCtrl", 0x1F803938); 

    // patch it
    sctrlHENPatchSyscall(hooked_readbuffer_func, sceCtrlReadBufferPositive_patch);
 
    return 0;
}

