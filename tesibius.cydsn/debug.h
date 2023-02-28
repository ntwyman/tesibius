#pragma once
#include <cytypes.h>
#define DEBUG_UART_ENABLED 1
    
void 
InitDebug();

#if DEBUG_UART_ENABLED
int
DebugPrintf(const char *format, ...);
#define DBG_PRINTF(format,...) DebugPrintf(format, ##__VA_ARGS__)

#else

#define DBG_PRINTF(format, ...)

#endif // DEBUG_UART_ENABLED

/* [] END OF FILE */
