#ifndef slog_h
#define slog_h

#include <stdlib.h>
#include <stdarg.h>
#include <stdio.h>
#include <string.h>

void Log(const char *file, const char *fun, int line, const char *msg, ...);
#define LOG(msg, ...)     Log(__FILE__, __FUNCTION__, __LINE__, msg, ## __VA_ARGS__)
#endif