#ifndef FILE_H
#define FILE_H

#include <constants.h>
#include <log.h>
#include <string.h>
#include <errno.h>
#include <unistd.h>

void
close_file(int, const char *);

#endif //FILE_H
