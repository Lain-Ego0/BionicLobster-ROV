#include <errno.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <unistd.h>

#include "usart.h"

extern char _end;
extern char _estack;

caddr_t _sbrk(int incr)
{
    static char *heap_end;
    char *prev_heap_end;

    if (heap_end == 0) {
        heap_end = &_end;
    }

    prev_heap_end = heap_end;
    if ((heap_end + incr) > (&_estack - 256)) {
        errno = ENOMEM;
        return (caddr_t)-1;
    }

    heap_end += incr;
    return (caddr_t)prev_heap_end;
}

int _write(int file, char *ptr, int len)
{
    if (file == STDOUT_FILENO || file == STDERR_FILENO) {
        for (int i = 0; i < len; ++i) {
            Serial_SendByte((uint8_t)ptr[i]);
        }
        return len;
    }

    errno = EBADF;
    return -1;
}

int _read(int file, char *ptr, int len)
{
    (void)ptr;
    (void)len;
    errno = (file == STDIN_FILENO) ? EAGAIN : EBADF;
    return -1;
}

int _close(int file)
{
    errno = EBADF;
    return -1;
}

off_t _lseek(int file, off_t ptr, int dir)
{
    (void)file;
    (void)ptr;
    (void)dir;
    return 0;
}

int _fstat(int file, struct stat *st)
{
    if (file == STDOUT_FILENO || file == STDERR_FILENO || file == STDIN_FILENO) {
        st->st_mode = S_IFCHR;
        return 0;
    }

    errno = EBADF;
    return -1;
}

int _isatty(int file)
{
    return (file == STDOUT_FILENO || file == STDERR_FILENO || file == STDIN_FILENO);
}
