#ifndef _DEBUG_H_
#define _DEBUG_H_

#ifdef DEBUG

#include <stdio.h>

#define DBGPRINT(_fmt, ...)  fprintf(stderr, "DEBUG [%s:%d@%s] " _fmt "\n", __FILE__, __LINE__, __func__, ##__VA_ARGS__)

#else // DEBUG

#define DBGPRINT(_fmt, ...)

#endif // DEBUG

#endif // _DEBUG_H_