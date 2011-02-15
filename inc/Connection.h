#ifndef EVENTIE_INC_CONNECTION_H
#define EVENTIE_INC_CONNECTION_H

#include <string>
#include <sstream>

class Connection {
public:
  Connection(int sock);
  Connection();
  void setSocket(int sock);
  bool readAndHandle();
private:
  std::string _buffer;
  int _sock;
};

#endif /* end of include guard: EVENTIE_INC_CONNECTION_H */
