#ifndef PIPE_H
#define PIPE_H

#include <types.h>

int64_t pipe(fd_t pipefd[2]);

int64_t pipeWrite(fd_t fd, const char * buffer, uint64_t bytes);

int64_t pipeRead(fd_t fd, char * buffer, uint64_t bytes);

int64_t pipeClose(fd_t fd);

int64_t addToPipe(fd_t fd);

int64_t namedPipe(const char *name, fd_t pipefd[2]);

int64_t unlinkpipe(const char *name);

TPipeInfo *pipeDump(uint64_t *size);
#endif//PIPE_H
