#include <stdbool.h>
#include <string.h>
#include <stdio.h>
#include <stdint.h>
#include <poll.h>
#include <stdlib.h>
#include <errno.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include "configure.h"
#include "ipc.h"
#define LONGEST_COMMAND 10

int fd = 0;
_Bool SPM_IPC_CreateFIFO(void){
    _Bool success = false;
    if(mkfifo(COMMUNICATION_FIFO_LOCATION, S_IRWXU) == 0 || errno == EEXIST){
        success = true;
        printf("%d\n", fd);
        fd = open(COMMUNICATION_FIFO_LOCATION, O_NONBLOCK, O_RDONLY);
        printf("%d\n", fd);
    }
    return success;
}

_Bool SPM_IPC_DeleteFIFO(void){
    _Bool success = false;
    close(fd);
    fd = 0;
    if(remove(COMMUNICATION_FIFO_LOCATION) == 0){
        success = true;
    }
    return success;
}

_Bool SPM_IPC_ExistsFIFO(void){
    _Bool success = false;
    if(mkfifo(COMMUNICATION_FIFO_LOCATION, S_IRWXU) != 0){
        if(errno == EEXIST){
            success = true;
        }
    }else{
        SPM_IPC_DeleteFIFO();
    }
    return success;
}

char *SPM_IPC_ReadFIFO(uint32_t *size){
    /* Commands
     * --------
     * STOP - Stops all processes
     * RESTART - Restart all processes
     * SHUTDOWN - Shutsdown
     */
    FILE *f = NULL;
    char *buff = NULL;
    char *_buff = NULL;
    uint32_t _size = 0;
    int err = 0;
    if((f = fdopen(fd, "r")) != NULL){
        if((buff = calloc(18, sizeof(char))) != NULL){
            uint32_t read = 0;
            _buff = buff;
            while(!(err = ferror(f)) && !feof(f) && *_buff != '\n'){
                if((read = fread(_buff, sizeof(char), 1, f)) == 1 && *_buff != '\n'){
                    _size++;
                    _buff++;
                }else if(read == 0){
                    continue;
                }else if(read > 1){
                    fprintf(stderr, "ERROR: Read more characters than expecting! Expected 1, got %d!\n", (int)read);
                    break;
                }
                if(_size >= 18){
                    if((buff = realloc(buff, _size + 18)) != NULL){
                        memset(buff += _size, 0, _size + 18);
                        _buff = buff;
                        _buff += _size;
                    }else{
                        err = -111111;
                        break;
                    }
                }
            }
        }
        *_buff = '\0';
        if(err){
            fprintf(stderr, "ERROR: Error occurred while reading! Error number is %d\n", err);
            fprintf(stderr, "Please report this to the developer with the error code!\n", err);
            _size = 0;
            free(buff);
            buff = NULL;
            _buff = NULL;
        }
        fclose(f);
        f = NULL;
    }
    *size = _size;
    return buff;
}

uint32_t SPM_IPC_WriteFIFO(char *input, uint32_t input_size){
    FILE *f = NULL;
    int written = 0;
    if((f = fopen(COMMUNICATION_FIFO_LOCATION, "w")) != NULL){
        if((written = fprintf(f, "%s\n", input)) < input_size){
            fprintf(stderr, "ERROR: Unable to write to FIFO!\n");
        }else if(written - 1 > input_size){
            fprintf(stderr, "ERROR: written size %d is greater than input size %d!\n", written, (int)input_size);
        }
        fclose(f);
        f = NULL;
    }
    return written;
}

_Bool SPM_IPC_CheckFIFO(void){
    _Bool success = false;
    struct pollfd _fd = {.fd = fd, .events = POLLRDNORM};
    struct pollfd fdarr[1] = {_fd};
    if(poll(fdarr, 1, 0) > 0){
        success = true;
    }
    return success;
}
