#include <string.h>
#include "../manager/manager.h"
#include "../process/process.h"
#include "../process/user_procs.h"

void user_setup(void){
    test_setup();
}

void test_setup(void){
    struct SPM_Command *start = NULL;
    struct SPM_Process *process = NULL;
    start = SPM_CreateCommand(strlen("python3 test.py") + 1, "python3 test.py");
    process = SPM_CreateProcess(start, NULL, NULL);
    SPM_Manager_AddProcess(process, START);
}
