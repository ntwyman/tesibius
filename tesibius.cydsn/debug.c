#include <stdarg.h>
#include <stdio.h>
#include <project.h>
#include "debug.h"

#if DEBUG_UART_ENABLED
void InitDebug()
{
    uart_debug_Start(); // Do we want to run this, even in production builds. Perhaps some way to turn it on with boot keys
}

int DebugPrintf(const char *format, ...)
{
    char buf[0x200];
    va_list args;
    va_start(args, format);
    int r = vsprintf(buf, format, args);  // Beware of buffer over-runs
    va_end(args);
    uart_debug_UartPutString(buf);
    return r;
}
#else
void InitDebug()
{
}
#endif
/* [] END OF FILE */
