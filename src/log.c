#include <log.h>

void
print_log(
        FILE *stream,
        const char *msg
) {
    const char *format = "[ %s ] [ SOURCE: %s | FUNCTION: %s() | LINE: %d ]\n(%s)\n";
    fprintf (stream, format, __DATE__, __FILE__, __FUNCTION__, __LINE__, msg);
}
