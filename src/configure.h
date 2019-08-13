#ifndef S_SPMCONFIGURE
#define S_SPMCONFIGURE

/* General Management */
#define TOTAL_MANAGED 10
#define COMMUNICATION_FIFO_LOCATION "/tmp/spm-ipc.fifo"

/* Feature Options */
/* TODO - Actually Implement these features */
#define DOUBLE_FORK false /* true = Double-Fork to Daemonize ; false = Do not double-fork */

/* Logging Information */
#define LOGGING SYSLOG    /* printf = Use printfs to log ; syslog = use syslog interfaces ; custom = use custom ; off = no logging */
#define LOGGING_NAME "Simple Process Manager"
#define LOG_LEVEL WARN  /* Logging Level (INFO, DEBUG, WARN, ERROR). See logging.h for more details */
#endif
