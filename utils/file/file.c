//
// Created by engineer on 30.09.22.
//
#include "file.h"

void
to_upper(
        char *s
) {
    for (int i = 0; s[i] != '\0'; ++i) {
        if (s[i] >= 'a' && s[i] <= 'z') {
            s[i] = (const char) (s[i] - 32);
        }
    }
}

