#include <stdlib.h>
#include <stdbool.h>
#include <string.h>
#include <strings.h>
#include <stdio.h>
#include <ctype.h>
#include "configure.h"
#include "process/user_procs.h"
#include "ipc.h"
#include "manager/manager.h"
#include "process/process.h"

int main(int argc, char *argv[]){
    _Bool run = true;
    unsigned char *lower_str = NULL;
    uint32_t i = 0;
    if(argc > 1){
        if(SPM_IPC_ExistsFIFO() == true){
            lower_str = argv[1];
            if(strcmp(lower_str, "stop") == 0){
                SPM_IPC_WriteFIFO("stop", strlen("stop"));
            }else if(strcmp(lower_str, "restart") == 0){
                SPM_IPC_WriteFIFO("restart", strlen("restart"));
            }else if(strcmp(lower_str, "shutdown") == 0){
                SPM_IPC_WriteFIFO("shutdown", strlen("shutdown"));
            }else{
                fprintf(stderr, "ERROR: Command %s unknown!\n", argv[1]);
            }
        }else{
            fprintf(stderr, "ERROR: UNABLE TO COMMUNICATE WITH MAIN PROCESS!\n");
        }
    }else{
        if(SPM_IPC_ExistsFIFO() == true){
            fprintf(stderr, "ERROR: FIFO at location %s exists!\n", COMMUNICATION_FIFO_LOCATION);
        }else{
            if(SPM_IPC_CreateFIFO() == true){
                struct SPM_Process *proc = NULL;
                _Bool has_stopped = false;
                user_setup();
                while(run){
                    if((proc = SPM_Manager_GetNextProcess(proc)) != NULL){
                        if(SPM_Manager_CheckProcess(proc) == FAILURE){
                            fprintf(stderr, "ERROR: Unable to check process with pid %d\n", (int)(SPM_GetPid(proc)));
                        }
                    }
                    if(SPM_IPC_CheckFIFO() == true){
                        char *cmd = NULL;
                        uint32_t size = 0;
                        if((cmd = SPM_IPC_ReadFIFO(&size)) != NULL){
                            printf("%s\n", cmd);
                            if(strcmp(cmd, "stop") == 0){
                               if(has_stopped == true){
                                   has_stopped = true;
                                   SPM_Manager_Stop();
                               }
                            }else if(strcmp(cmd, "restart") == 0){
                                if(has_stopped == true){
                                    user_setup();
                                    continue;
                                }
                            }else if(strcmp(cmd, "shutdown") == 0){
                                run = false;
                            }
                        }
                        free(cmd);
                        cmd = NULL;
                    }
                }
                if(!has_stopped){
                    SPM_Manager_Stop();
                }
                SPM_IPC_DeleteFIFO();
            }else{
                fprintf(stderr, "ERROR: Unable to create FIFO at location %s!\n", COMMUNICATION_FIFO_LOCATION);
            }
        }
    }
    return 0;
}
