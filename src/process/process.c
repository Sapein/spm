#include <stdio.h>
#include <stdbool.h>
#include <stdlib.h>
#include <sys/wait.h>
#include <sys/types.h>
#include <signal.h>
#include <unistd.h>
#include <string.h>
#include <strings.h>
#include "../configure.h"
#include "../logging.h"
#include "process.h"

struct SPM_Command {
    uint32_t size;
    char command[];
};

#if (NAMED_PROCS != true)
struct SPM_Process {
    pid_t process_id;
    enum SPM_ProcessStatus CurrentStatus;
    struct SPM_Command *start;
    struct SPM_Command *stop;
    struct SPM_Command *restart;
};
#else
struct SPM_Process {
    pid_t process_id;
    uint32_t name_len;
    enum SPM_ProcessStatus CurrentStatus;
    struct SPM_Command *start;
    struct SPM_Command *stop;
    struct SPM_Command *restart;
    char name[];
};
#endif

struct SPM_Command *SPM_CreateCommand(uint32_t command_len, char command[]){
    struct SPM_Command *new_command = NULL;
    if((new_command = calloc(1, sizeof(struct SPM_Command) + (command_len * sizeof(char))) ) != NULL){
        new_command->size = command_len;
        if(snprintf(new_command->command, command_len, "%s", command) < (int) command_len - 1){
            SPM_Log(ERROR, "COMMAND %s IS UNABLE TO BE USED BECAUSE IT IS TRUNCATED!\n", command);
            free(new_command);
            new_command = NULL;
        }
    }
    return new_command;
}

void SPM_DestroyCommand(struct SPM_Command *command){
    free(command);
}

struct SPM_Process *SPM_CreateProcess(struct SPM_Command *start, struct SPM_Command *stop, struct SPM_Command *restart){
    struct SPM_Process *new_proc = NULL;
    struct SPM_Command *command_buff = NULL;
    if((new_proc = calloc(1, sizeof(struct SPM_Process))) != NULL){
        new_proc->start = NULL;
        new_proc->stop = NULL;
        new_proc->restart = NULL;
        new_proc->process_id = -1;
        new_proc->CurrentStatus = CREATED;
        if(start != NULL  && (command_buff = SPM_CreateCommand(start->size, start->command)) != NULL){
            new_proc->start = command_buff;
            command_buff = NULL;
            if(stop != NULL && (command_buff = SPM_CreateCommand(stop->size, stop->command)) != NULL){
                new_proc->stop = command_buff;
                command_buff = NULL;
            }else if(stop != NULL){
                free(new_proc->start);
                free(new_proc);
                new_proc = NULL;
                command_buff = NULL;
                goto end;
            }
            if(restart != NULL && (command_buff = SPM_CreateCommand(restart->size, restart->command)) != NULL){
                new_proc->restart = command_buff;
                command_buff = NULL;
            }else if(restart != NULL){
                free(new_proc->start);
                if(new_proc->stop != NULL){
                    free(new_proc->stop);
                }
                command_buff = NULL;
                new_proc = NULL;
                goto end;
            }
        }else{
            free(new_proc);
            new_proc = NULL;
        }
    }
end:
    return new_proc;
}

void SPM_DestroyProcess(struct SPM_Process *proc){
    if(proc != NULL){
        free(proc->start);
        if(proc->stop != NULL){
            free(proc->stop);
        }
        if(proc->restart != NULL){
            free(proc->restart);
        }
        proc->start = NULL;
        proc->restart = NULL;
        proc->stop = NULL;
        free(proc);
        proc = NULL;
    }
}

enum SPM_ProcessStatus SPM_GetStatus(struct SPM_Process *proc){
    return proc->CurrentStatus;
}

int _exec(struct SPM_Command *command, _Bool exit_immediately){
    int return_value = 0;
    return_value = system(command->command);
    if(exit_immediately == true){
        exit(return_value);
    }
    return return_value;
}

enum SPM_Result SPM_ChangeStatus(struct SPM_Process *proc, enum SPM_ProcessStatus new_status){
    enum SPM_Result result = FAILURE;
    pid_t proc_id = -1;
    if(proc != NULL){
        if(proc->CurrentStatus != new_status && (new_status != CREATED || new_status != UNK)){
            switch(new_status){
                case CREATED:
                case UNK:
                    break;
#pragma GCC diagnostic push
#pragma GCC diagnostic ignored "-Wimplicit-fallthrough"
                case RESTART:
                    if(proc->restart == NULL){
                        result = NORESTART;
                        break;
                    }
                case START:
                    proc_id = fork();
                    switch(proc_id){
#pragma GCC diagnostic pop
                        case 0:
                            _exec(proc->start, true);
                            break;
                        case -1:
                            SPM_Log(ERROR, "UNABLE TO START CHILD!\n");
                            break;
                        default:
                            proc->process_id = proc_id;
                            proc->CurrentStatus = START;
                            result = SUCCESS;
                            break;
                    }
                    break;
                case STOP:
                    if(proc->stop != NULL && (proc->CurrentStatus != CREATED && proc->CurrentStatus != UNK)){
                        switch(_exec(proc->stop, false)){
                            case 127:
                            case -1:
                                if(proc->process_id > 1){
                                    kill(proc->process_id, SIGKILL);
                                }
                                proc->CurrentStatus = STOP;
                                break;
                            default:
                                for(uint8_t loop_count = 0; loop_count < 100; loop_count++){
                                    switch(waitpid(proc->process_id, NULL, WNOHANG)){
                                        case 0:
                                            if(loop_count == 99 && proc->process_id > 1){
                                                proc->CurrentStatus = STOP;
                                                kill(proc->process_id, SIGKILL);
                                            }
                                            break;
                                        default:
                                            result = SUCCESS;
                                            loop_count = 100;
                                    }
                                }
                        }
                    }else if(proc->CurrentStatus != CREATED && proc->CurrentStatus != UNK){
                        if(proc->process_id > 1){
                            kill(proc->process_id, SIGKILL);
                            result = SUCCESS;
                            proc->CurrentStatus = STOP;
                        }
                    }else if(proc->CurrentStatus == CREATED){
                        proc->CurrentStatus = STOP;
                        result = SUCCESS;
                    }
                    break;
            }
        }else if(proc->CurrentStatus == new_status){
            result = NOCHANGE;
        }else{
            result = BADSTATUS;
        }
    }
    return result;
}

void SPM_CheckStatus(struct SPM_Process *proc){
    if(proc->CurrentStatus != UNK && proc->CurrentStatus != CREATED){
        switch(waitpid(proc->process_id, NULL, WNOHANG)){
            case 0:
                proc->CurrentStatus = START;
                break;
            case -1:
                if(proc->CurrentStatus != STOP && proc->CurrentStatus != CREATED){
                    proc->process_id = -1;
                    proc->CurrentStatus = UNK;
                }
                break;
            default:
                proc->process_id = -1;
                proc->CurrentStatus = STOP;
                break;
        }
    }
}

pid_t SPM_GetPid(struct SPM_Process *proc){
    return proc->process_id;
}

#pragma GCC diagnostic push
#if (NAMED_PROCS != true)
#pragma GCC diagnostic ignored "-Wunused-parameter"
#endif
uint32_t SPM_GetName(struct SPM_Process *proc, char *name_out){
#pragma GCC diagnostic pop
#if (NAMED_PROCS == true)
    uint32_t name_len = 0;
    if(proc != NULL){
        if(proc->name_len != strlen(proc->name)){
            if(proc->name_len > strlen(proc->name)){
                name_len = proc->name_len;
            }else{
                name_len = strlen(proc->name);
            }
        }
        if(name_out != NULL && name_len > 0){
            if(strncpy(name_out, proc->name, name_len) == NULL){
                SPM_Log(ERROR, "Unable to copy name %s to name_out!\n", proc->name);
                name_out = memset(name_out, 0, sizeof(char));
            }
        }
    }
    return name_len;
#else
    return 0;
#endif
}

#pragma GCC diagnostic push
#if (NAMED_PROCS != true)
#pragma GCC diagnostic ignored "-Wunused-parameter"
#endif
struct SPM_Process *SPM_NameProcess(char *name, uint32_t name_len, struct SPM_Process *proc){
#pragma GCC diagnostic pop
#if (NAMED_PROCS == true)
    struct SPM_Process *named_process = NULL;
    if(name != NULL && name_len > 0){
        if((named_process = calloc(1, sizeof(struct SPM_Process) + (name_len * sizeof(char))) ) != NULL){
            if(memcpy(named_process, proc, sizeof(struct SPM_Process)) != NULL){
                named_process->name_len = name_len;
                if(strncpy(named_process->name, name, name_len) != NULL){
                    named_process = NULL;
                }
            }else{
                free(named_process);
            }
        }
    }
    return named_process;
#else
    return NULL;
#endif
}
