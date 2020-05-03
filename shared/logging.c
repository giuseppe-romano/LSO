#include <stdio.h>
#include <stdlib.h>
#include <time.h>

FILE *file = NULL;
char* filename = "activity.log";
void initLogFile(char* _filename)
{
    if(_filename)
    {
        filename = _filename;
    }

    if (file == NULL) {
        file = fopen(filename, "a+"); // a+ (create + append)
        fprintf (file, "\n\n\n");
    }
}

char* timestamp()
{
    char* buff = (char*)calloc(100, sizeof(char));
    time_t now = time (0);
    strftime (buff, 100, "%d-%m-%Y %H:%M:%S", localtime (&now));
    return buff;
}

void _log(char* category, char* message, char* level)
{
    initLogFile(NULL);
    char* t = timestamp();
    fprintf (file, "[%s][%s] - [%s] - %s\n", t, level, category, message);
    fflush(file);

    free(t);
}

void info(char* category, char* message)
{
    _log(category, message, "INFO");
}

void warn(char* category, char* message)
{
    _log(category, message, "WARN");
}

void error(char* category, char* message)
{
    _log(category, message, "ERROR");
}

void infoMain(char* message)
{
    info("MAIN", message);
}
void warnMain(char* message)
{
    warn("MAIN", message);
}
void errorMain(char* message)
{
    error("MAIN", message);
}
void infoSerial(char* message)
{
    info("SERIAL", message);
}
void infoPlayer(char* message)
{
    info("PLAYER", message);
}
void warnPlayer(char* message)
{
    warn("PLAYER", message);
}
void infoProtocol(char* message)
{
    info("PROTOCOL", message);
}
void infoGame(char* message)
{
    info("GAME", message);
}
void warnGame(char* message)
{
    warn("GAME", message);
}
void infoMenu(char* message)
{
    info("MENU", message);
}
void infoDraw(char* message)
{
    info("DRAW", message);
}
