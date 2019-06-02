#ifndef S_SPMUSERPROCS
#define S_SPMUSERPROCS
#define Create_Command(x) SPM_CreateCommand(strlen( x ) + 1, x)
#define Process(id) Process_Create##id()
#define Create_Process(id, start_, stop_, restart_) \
    void Process_Create##id(void){ \
        struct SPM_Command *start = NULL; \
        struct SPM_Command *stop = NULL; \
        struct SPM_Command *restart = NULL; \
        struct SPM_Process *process = NULL; \
        start = SPM_CreateCommand(strlen((start_)) + 1, (start_) ); \
        if( (stop_) != NULL){ \
            stop = SPM_CreateCommand(strlen( (stop_) ) + 1, (stop_) ); \
        } \
        if( (restart_) != NULL) { \
            restart = SPM_CreateCommand(strlen( (restart_) ) + 1, (restart_) ); \
        }\
        process = SPM_CreateProcess(start, stop, restart);\
        SPM_Manager_AddProcess(process, START); \
    }\

void user_setup(void);
#endif
