#ifndef slog_h
#define slog_h
void Log( const char* file, const char* fun, int line, const char* msg, ...)
{
    va_list arg_ptr;
    va_start(arg_ptr, msg);
    char buf[1024];
    int len = 0;
    memset(&buf[0], 0, sizeof(buf));
    vsnprintf(buf, sizeof(buf), msg, arg_ptr);

    char final_buf[2048];
    snprintf_s(final_buf, sizeof(final_buf), len, 
        "[%lu]%s %s %d %s\n",  pthread_self(), file, fun, line, buf);
    printf("%s\n", final_buf);
}
#define LOG(msg, ...)     Log(__FILE__, __FUNCTION__, __LINE__, msg, ## __VA_ARGS__)
#endif