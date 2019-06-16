#ifndef S_SPMLOGGING
#define S_SPMLOGGING
#include "configure.h"

void SPM_LogStart(void);
void SPM_LogStop(void);
void SPM_Log(int level, const char *msg, ...);

#define CUSTOM 1
#define SYSLOG 2
#define PRINTF 3
#define OFF 4


#define EMPTY()
#define DEFER(x) x EMPTY()
#if (DEFER(LOGGING) == SYSLOG)
#include <syslog.h>
#define ERROR DEFER(LOG_ERR)
#define WARN  DEFER(LOG_WARNING)
#define INFO  DEFER(LOG_INFO)
#define DEBUG DEFER(LOG_DEBUG)
#elif (DEFER(LOGGING) != OFF)
#define ERROR 13
#define WARN  12
#define INFO  11
#define DEBUG 10
#endif
#endif
