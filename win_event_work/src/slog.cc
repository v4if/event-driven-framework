#include "slog.h"
void snprintf_s(char*buf, int _max, int& len, const char* fmt, ...)
{
    if (len >= 0 && len < _max)
    {
        va_list arg;
        va_start(arg, fmt);
        len += vsnprintf(buf + len, _max - len, fmt, arg);
        va_end(arg);
    }
    buf[_max - 1] = 0;
}
void Log(const char *file, const char *fun, int line, const char *msg, ...)
{
    va_list arg_ptr;
    va_start(arg_ptr, msg);
    char buf[1024];
    int len = 0;
    memset(&buf[0], 0, sizeof(buf));
    vsnprintf(buf, sizeof(buf), msg, arg_ptr);

    char final_buf[2048];
    snprintf_s(final_buf, sizeof(final_buf), len, 
        "%s %s:%d %s\n",  file, fun, line, buf);
    printf("%s\n", final_buf);
}