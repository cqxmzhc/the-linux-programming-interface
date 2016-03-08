#include <string.h>
#include <stdlib.h>
#include <malloc.h>
#include "../tlpi-dist/lib/tlpi_hdr.h"

extern char **environ;
char *envStr;

int my_setenv(const char *name, const char *value, int overrite) {
    char *tmpStr = getenv(name);
    if (tmpStr && overrite) {
        putenv(tmpStr);
        return 0;
    }
    else if (tmpStr && overrite == 0) {
        return 0;
    }
    else {
        int len = strlen(name) + strlen(value);
        char *envStr = malloc(len+2);
        strcat(envStr, name);
        strcat(envStr, "=");
        strcat(envStr, value);
        //environ[sizeof(environ)/sizeof(*environ)] = envStr;
        putenv(envStr);
    }

    return 0;
}

int my_unsetenv(const char *name) {
    char *tmpStr;
    char **envPos;
    char **tmpenv = environ;
    if ((tmpStr = getenv(name))) {
       for (tmpenv; *tmpenv != NULL; ++tmpenv) {
            if (strncmp(*tmpenv, name, (size_t) strlen(name)) == 0 && (*tmpenv)[strlen(name)] == '='){
                envPos = tmpenv;
                while(*(++tmpenv) != NULL);
                *envPos = *(--tmpenv);
                *tmpenv = NULL;
            }
       }
    }
}

int
main (int argc, char **argv) {
    my_setenv("testhaha", "hahatest", 0);
    char *env = getenv("testhaha");
    printf("testhaah is %s\n", env);
    my_unsetenv("testhaha");
    printf("testhaha is %s\n", getenv("testhaha"));
}

