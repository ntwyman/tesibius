/*
System semi-hosting stubs to allow printf() to work, and send to debug UART.
*/
#include <errno.h>
#include <sys/stat.h>
#include <sys/unistd.h>

#include "project.h"

int _close(int _file)
{
	return -1;
}

int _fstat(int _file, struct stat *st)
{
	st->st_mode = S_IFCHR;
	return 0;
}

int _isatty(int file)
{
	switch(file) {
		case STDOUT_FILENO:
		case STDERR_FILENO:
		case STDIN_FILENO:
			return 1;
		default:
			errno = EBADF;
			return 0;
	}
}

int _lseek(int file, int ptr, int dir)
{
	return 0;
}

int _read(int file, char *ptr, int len)
{
	return 0;
}

int _write(int file, char *ptr, int len)
{
    switch(file) 
    {
    case STDOUT_FILENO:
    case STDERR_FILENO:			
        for (int n = 0; n < len; n++)
        {
            uart_debug_UartPutChar(*ptr++);
        }        
        break;
    
    default:
        errno = EBADF;
        return -1;
    }
    return len;
}
