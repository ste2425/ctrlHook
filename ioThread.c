#include "ioThread.h"

int ioThread(SceSize args, void *argp)
{    
    // dont do anything until we can confirm all modules are loaded
    waitForKernel();    

    // Initialize the psp-uart-library
    pspUARTInit(UART_BAUD_RATE);

    while (1) {
        pspUARTWrite(COMMANDS_RIGHT_ANALOG);

        pspUARTWaitForData(10000);

        int recievedDataCount = pspUARTAvailable();

        // If we recieved no data skip and request again
        if (recievedDataCount == 0) 
        {
            continue;
        } 
        // If only recieved first byte, wait for second
        else if (recievedDataCount == 1)
        {
            pspUARTWaitForData(10000);
        }
        
        // If still not recieved expected two bytes by this point
        // reset any data recieved and request again
        if (pspUARTAvailable() != 2) 
        {
            pspUARTResetRingBuffer();
            continue;
        }
        
        int valueX = pspUARTRead();
        int valueY = pspUARTRead();

        if (valueX != -1 && valueY != -1) {
            xAxis = valueX;
            yAxis = valueY;
        }
    }
 
    return 0;
}