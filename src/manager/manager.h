#ifndef S_SPMMANAGER
#include "../process/process.h"
#define S_SPMANAGER

enum SPM_Manager_Results {SUCCESS_, FAIL, NO};
void SPM_Manager_Stop(void);

enum SPM_Manager_Results SPM_Manager_AddProcess(struct SPM_Process *proc, enum SPM_ProcessStatus MaintainStatus);
struct SPM_Process *SPM_Manager_GetNextProcess(struct SPM_Process *proc);
enum SPM_Manager_Results SPM_Manager_CheckProcess(struct SPM_Process *proc);
#endif
