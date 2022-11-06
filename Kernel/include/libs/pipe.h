#ifndef PIPE_H
#define PIPE_H

#include <types.h>

int64_t pipe(fd_t pipefd[2]);

int64_t pipeWrite(fd_t fd, const char * buffer, uint64_t bytes);

int64_t pipeRead(fd_t fd, char * buffer, uint64_t bytes);

int64_t pipeClose(fd_t fd);

#endif//PIPE_H
