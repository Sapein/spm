#include <stdlib.h>
#include "manager.h"
#include <stdio.h>
#include "../configure.h"

struct _Proc {
    enum SPM_ProcessStatus intended_status;
    struct SPM_Process *actual_process;
};

struct _Proc *_process[TOTAL_MANAGED] = {NULL};
enum SPM_Manager_Results SPM_Manager_Init(void){
    return SUCCESS_;
}
void SPM_Manager_Stop(void){
    for(int i = 0; i < TOTAL_MANAGED; i++){
        struct _Proc *proc = NULL;
        if(_process[i] != NULL){
            proc = _process[i];
            _process[i] = NULL;
            SPM_CheckStatus(proc->actual_process);
            SPM_ChangeStatus(proc->actual_process, STOP);
            SPM_DestroyProcess(proc->actual_process);
            proc->actual_process = NULL;
            free(proc);
            proc = NULL;
        }
    }
}
enum SPM_Manager_Results SPM_Manager_AddProcess(struct SPM_Process *proc, enum SPM_ProcessStatus MaintainStatus){
    enum SPM_Manager_Results results = FAIL;
    struct _Proc new_proc = {.actual_process = proc, .intended_status = MaintainStatus};
    struct _Proc *_proc = NULL;
    for(int i = 0; i < TOTAL_MANAGED; i++){
        if(_process[i] == NULL){
            _process[i] = &new_proc;
            if((_proc = calloc(1, sizeof(struct _Proc))) != NULL){
                _proc->actual_process = new_proc.actual_process;
                _proc->intended_status = new_proc.intended_status;
                _process[i] = _proc;
                results = SUCCESS_;
            }
            break;
        }
    }
    return results;
}

enum SPM_Manager_Results SPM_Manager_CheckProcess(struct SPM_Process *proc){
    enum SPM_Manager_Results result = FAIL;
    enum SPM_ProcessStatus current_status = UNK;
    for(int i = 0; i < TOTAL_MANAGED; i++){
        if(_process[i]->actual_process == proc){
            SPM_CheckStatus(proc);
            current_status = SPM_GetStatus(proc);
            if(current_status != _process[i]->intended_status){
                if(current_status == STOP){
                    if(SPM_ChangeStatus(proc, RESTART) != FAILURE){
                        result = SUCCESS_;
                    }
                }else if(current_status == CREATED){
                    if(SPM_ChangeStatus(proc, START) != FAILURE){
                        result = SUCCESS_;
                    }
                }
            }else{
                result = SUCCESS_;
            }
            break;
        }
    }
    return result;
}
struct SPM_Process *SPM_Manager_GetNextProcess(struct SPM_Process *proc){
    struct SPM_Process *out_proc = NULL;
    if(proc != NULL){
        for(int i = 0; i < TOTAL_MANAGED; i++){
            if(_process[i] != NULL && _process[i]->actual_process == proc){
                if((i + 1) < TOTAL_MANAGED && _process[i+1] != NULL){
                    out_proc = _process[i+1]->actual_process;
                    break;
                }else{
                    out_proc = _process[0]->actual_process;
                    break;
                }
            }
        }
    }else{
        if(_process[0] != NULL){
            out_proc = _process[0]->actual_process;
        }
    }
    return out_proc;
}
