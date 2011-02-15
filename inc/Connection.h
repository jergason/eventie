#ifndef EVENTIE_INC_CONNECTION_H
#define EVENTIE_INC_CONNECTION_H

#include <string>
#include <iostream>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <errno.h>
#include <cstring>
#include <stdio.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <sys/sendfile.h>
#include <unistd.h>

#include "Handler.h"


class Connection {
public:
  Connection(int sock);
  Connection();
  void setSocket(int sock);
  bool readAndHandle();
private:
  std::string _buffer;
  int _sock;
  int BUFFER_SIZE;
  static std::string SENTINEL;


  bool send(std::string& message);
};

#endif /* end of include guard: EVENTIE_INC_CONNECTION_H */
