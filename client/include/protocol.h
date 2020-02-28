#ifndef PROTOCOL_HEADERFILE_INCLUDED
#define PROTOCOL_HEADERFILE_INCLUDED

void setServerSocket(int _socket);

void sendRegisterRequest(char *username, char *password);
void sendLoginRequest(char *username, char *password);

#endif
