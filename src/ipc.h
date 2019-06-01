#ifndef S_SPMIPC
#include <stdint.h>
#define S_SPMIPC

_Bool SPM_IPC_CreateFIFO(void);
_Bool SPM_IPC_DeleteFIFO(void);
_Bool SPM_IPC_ExistsFIFO(void);
_Bool SPM_IPC_CheckFIFO(void);
char *SPM_IPC_ReadFIFO(uint32_t *size_out);
uint32_t SPM_IPC_WriteFIFO(char *input, uint32_t input_size);
#endif
