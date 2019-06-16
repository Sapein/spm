#include <stdarg.h>
#include <stdio.h>
#include <syslog.h>
#include "logging.h"

#if (DEFER(LOGGING) == CUSTOM || DEFER(LOGGING) == PRINTF)
static FILE *log_file = NULL;
static int maxlog = LOG_LEVEL;
static char *level_strs[] = {[DEBUG] = "DEBUG", [INFO] = "INFO", [WARN] = "WARN", [ERROR] = "ERROR"};
#endif

void SPM_LogStart(void){
#if (DEFER(LOGGING) == SYSLOG)
    int mask = LOG_UPTO(LOG_LEVEL);
    openlog(LOGGING_NAME, 0, LOG_DAEMON);
    setlogmask(mask);
#elif (DEFER(LOGGING) == CUSTOM)
    if((log_file = fopen(LOGGING_NAME ".log", "w")) == NULL){
        fprintf(stderr, "ERROR! Unable to open Log file!\n");
        log_file = stderr;
    }
#elif (DEFER(LOGGING) == PRINTF)
    log_file = stderr;
#endif
}

void SPM_LogStop(void){
#if (DEFER(LOGGING) == SYSLOG)
    closelog();
#elif (DEFER(LOGGING) == CUSTOM)
    if(log_file != stderr && log_file != NULL){
        fclose(log_file);
    }
#endif
}

void SPM_Log(int level, const char *msg, ...){
#if (DEFER(LOGGING) != off)
    va_list vargs;
    va_start(vargs, msg);
#if (DEFER(LOGGING) == SYSLOG)
    syslog(level, msg, vargs);
#elif (DEFER(LOGGING) == CUSTOM || DEFER(LOGGING) == PRINTF)
    if(LOG_LEVEL < maxlog){
        fprintf(log_file,  level_strs[LOG_LEVEL] ": " msg, vargs);
    }
#endif
    va_end(vargs);
#endif
}
