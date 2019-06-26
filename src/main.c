#include <ctype.h>
#include <stdbool.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <strings.h>

#include "logging.h"
#include "ipc.h"
#include "manager/manager.h"
#include "process/process.h"
#include "user_processes/user_procs.h"

_Bool CommandParse(int argc, char *argv[]);
_Bool Initalize(void);

int main(int argc, char *argv[]){
    _Bool run = true;
    struct SPM_Process *proc = NULL;
    _Bool has_stopped = false;
    char *cmd = NULL;
    uint32_t size = 0;
    SPM_LogStart();
#if (IND_STOP == true)
    char *desired_proc_name = NULL;
    char *proc_name = NULL;
    uint32_t name_len = 0;
    struct SPM_Process *_proc = NULL;
#endif

#if (DOUBLE_FORK == true)
    switch(pid_t pid = fork()){
        case -1:
            SPM_Log(ERROR, "Unable to double-fork! exiting!\n");
            exit(1);
            break;
        case 0:
            switch(pid_t pid = fork()){
                case -1:
                    SPM_Log(ERROR, "Unable to double-fork! exiting!\n");
                    exit(1);
                case 0:
                    break;
                default
                    exit(0);
            }
        default:
            exit(0);
    }
#endif

    if(CommandParse(argc, argv) == true){
        if(Initalize() == true){
            while(run){
                if((proc = SPM_Manager_GetNextProcess(proc)) != NULL){
                    if(SPM_Manager_CheckProcess(proc) == FAIL){
                        SPM_Log(WARN, "Unable to check process with pid %d\n", (int)(SPM_GetPid(proc)));
                    }
                }
                if(SPM_IPC_CheckFIFO() == true){
                    if((cmd = SPM_IPC_ReadFIFO(&size)) != NULL){
#if (IND_STOP != true)
                        if(strcmp(cmd, "stop") == 0){
                            if(has_stopped == false){
#else
                        if(strncmp(cmd, "stop", strlen("stop")) == 0){
                            if(strcmp(cmd, "stop") != 0){
                                if((desired_proc_name = calloc(size - strlen("stop"), sizeof(char))) != NULL){
                                    if(sscanf(cmd, "stop %s", &proc_name) == strlen(cmd)){
                                        if((name_len = SPM_GetName(proc, NULL)) > 0){
                                            do{
                                                if((proc_name = calloc(name_len, sizeof(char))) != NULL){
                                                    SPM_GetName(_proc, proc_name);
                                                    if(strlen(proc_name, desired_proc_name) == 0){
                                                        SPM_ChangeStatus(_proc, STOP);
                                                        free(proc_name);
                                                        proc_name = NULL;
                                                        name_len = 0;
                                                        break;
                                                    }
                                                    free(proc_name);
                                                    proc_name = NULL;
                                                }
                                                _proc = SPM_Manager_GetNextProcess(_proc);
                                            }while(_proc != proc)
                                        }
                                    }
                                    free(desired_proc_name);
                                    desired_proc_name = NULL;
                                }
                            }else{
#endif
                                has_stopped = true;
                                SPM_Manager_Stop();
                            }
                        }else if(strcmp(cmd, "restart") == 0){
                            if(has_stopped == true){
                                user_setup();
                                has_stopped = false;
                                continue;
                            }else{
                                struct SPM_Process *p = proc;
                                enum SPM_ProcessStatus res = SUCCESS;
                                do{
                                    if((res = SPM_ChangeStatus(p, RESTART)) != FAILURE || res != NOMEM){
                                        SPM_Log(WARN, "UNABLE TO RESTART PROCESS WITH PID %d\n", SPM_GetPid(proc));
                                    }else if(res == NORESTART){
                                        SPM_Log(INFO, "PROCESS WITH PID %d HAS NO RESTART COMMAND!\n", SPM_GetPid(proc));
                                    }
                                    p = SPM_Manager_GetNextProcess(p);
                                }while(p != proc);
                                p = NULL;
                            }
                        }else if(strcmp(cmd, "shutdown") == 0){
                            run = false;
                        }
                        free(cmd);
                    }
                    cmd = NULL;
                }
            }
            if(has_stopped == false){
                SPM_Manager_Stop();
            }
            SPM_IPC_DeleteFIFO();
        }
    }
    SPM_LogStop();
    return 0;
}

_Bool CommandParse(int argc, char *argv[]){
    _Bool success = false;
    char *lower_str = NULL;
    if(SPM_IPC_ExistsFIFO() == true && argc > 1){
        success = true;
        lower_str = argv[1];
#if (IND_STOP != true)
        if(strcmp(lower_str, "stop") == 0){
            SPM_IPC_WriteFIFO("stop", strlen("stop"));
#else
        if(strncmp(lower_str, "stop", strlen("stop")) == 0){
            SPM_IPC_WriteFIFO(lower_str, strlen(lower_str));
#endif
        }else if(strcmp(lower_str, "restart") == 0){
            SPM_IPC_WriteFIFO("restart", strlen("restart"));
        }else if(strcmp(lower_str, "shutdown") == 0){
            SPM_IPC_WriteFIFO("shutdown", strlen("shutdown"));
        }else{
            SPM_Log(WARN, "Command %s unknown!\n", argv[1]);
            success = false;
        }
    }else if(argc > 1){
        SPM_Log(ERROR, "UNABLE TO COMMUNICATE WITH MAIN PROCESS!\n");
    }
    return success;
}

_Bool Initalize(void){
    _Bool success = false;
    if(SPM_IPC_ExistsFIFO() == true){
        SPM_Log(ERROR, "FIFO AT LOCATION %s EXISTS!\n", COMMUNICATION_FIFO_LOCATION);
    }else{
        if(SPM_IPC_CreateFIFO() == true){
            user_setup();
            success = true;
        }else{
            SPM_Log(ERROR, "UNABLE TO CREATE FIFO AT LOCATION %s\n", COMMUNICATION_FIFO_LOCATION);
        }
    }
    return success;
}

#if (IND_STOP == true && NAMED_PROCS != true)
#error "IND_STOP and NAMED_PROCS must be true!"
#endif
