//
// Created by engineer on 30.09.22.
//

#include "log.h"

void
print_log(
        FILE *stream,
        const char *log_tpl,
        const char *date, const char *source_file, const char *function, const int line,
        const char *msg
) {
    fprintf (stream, log_tpl, date, source_file, function, line, msg);
}