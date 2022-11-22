#include <log.h>
#include <time.h>
#include <string.h>

char *
get_cur_time() {
    time_t raw_time;
    struct tm * time_info;

    time ( &raw_time );
    time_info = localtime ( &raw_time );

    char *log_time = asctime (time_info);
    log_time[strlen(log_time) - 1] = '\0';
    return log_time;
}

void
print_log(
        FILE *stream,
        const char *bin_name,
        const char *msg
) {
    fprintf (stream, "%s %s: %s\n", get_cur_time(), bin_name, msg);
}
