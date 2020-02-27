#ifndef PROTOCOL_HEADERFILE_INCLUDED
#define PROTOCOL_HEADERFILE_INCLUDED

void setServerSocket(int _socket);

void sendRegisterAction(char *username, char *password);
void sendLoginAction(char *username, char *password);

#endif
