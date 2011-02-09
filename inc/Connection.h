#ifndef EVENTIE_INC_CONNECTION_H
#define EVENTIE_INC_CONNECTION_H

#include <string>
#include <sstream>

class Connection {
public:
  Connection(int sock);
private:
  std::stringstream _buffer;
  int _sock;
};

#endif /* end of include guard: EVENTIE_INC_CONNECTION_H */
