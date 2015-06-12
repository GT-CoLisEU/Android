#ifndef STDIO_TEMFILE_H
#define STDIO_TEMFILE_H

#include <sys/types.h>
#include <sys/stat.h>
#include <unistd.h>
#include <signal.h>
#include <errno.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <paths.h>


FILE * stdio_tempfile(void);


#endif

