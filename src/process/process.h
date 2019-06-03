#ifndef S_SPMPROCESS
#include <sys/types.h>
#include <stdint.h>
#define S_SPMPROCESS

enum SPM_ProcessStatus {STOP, START, RESTART, CREATED, UNK};
enum SPM_Result {SUCCESS, FAILURE, NOMEM, NOCHANGE, BADSTATUS, NORESTART};
struct SPM_Command;
struct SPM_Process;

struct SPM_Command *SPM_CreateCommand(uint32_t command_len, char command[]);
void SPM_DestroyCommand(struct SPM_Command *);

struct SPM_Process *SPM_CreateProcess(struct SPM_Command *start, struct SPM_Command *stop, struct SPM_Command *restart);
void SPM_DestroyProcess(struct SPM_Process *Proc);

void SPM_CheckStatus(struct SPM_Process *proc);
enum SPM_ProcessStatus SPM_GetStatus(struct SPM_Process *proc);
enum SPM_Result SPM_ChangeStatus(struct SPM_Process *proc, enum SPM_ProcessStatus newstatus);
pid_t SPM_GetPid(struct SPM_Process *proc);
#endif
