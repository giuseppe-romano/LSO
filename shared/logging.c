#include <stdio.h>
#include <stdlib.h>
#include <time.h>

FILE *file = NULL;
void initLogFile(char* filename)
{
    if (file == NULL) {
        file = fopen(filename, "a+"); // a+ (create + append)
    }
}

char* timestamp()
{
    char* buff = (char*)calloc(100, sizeof(char));
    time_t now = time (0);
    strftime (buff, 100, "%d-%m-%Y %H:%M:%S", localtime (&now));
    return buff;
}

void _log(char* message, char* level)
{
    fprintf (file, "[%s][%s] - %s\n", timestamp(), level, message);
    fflush(file);
}

void info(char* message)
{
    _log(message, "INFO");
}

void warn(char* message)
{
    _log(message, "WARN");
}

void error(char* message)
{
    _log(message, "ERROR");
}
