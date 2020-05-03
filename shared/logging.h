#ifndef LOGGING_HEADERFILE_INCLUDED
#define LOGGING_HEADERFILE_INCLUDED

void initLogFile(char* filename);
void info(char* category, char* message);
void warn(char* category, char* message);
void error(char* category, char* message);

void infoMain(char* message);
void warnMain(char* message);
void errorMain(char* message);
void infoSerial(char* message);
void infoPlayer(char* message);
void warnPlayer(char* message);
void infoProtocol(char* message);
void infoGame(char* message);
void warnGame(char* message);
void infoMenu(char* message);
void infoDraw(char* message);

#endif

