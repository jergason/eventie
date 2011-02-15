#include "Connection.h"

using namespace std;

Connection::Connection(int sock) {
  _sock = sock;
  _buffer = "";
}

Connection::Connection() {
  _sock = -1;
}

void Connection::setSocket(int sock) {
  _sock = sock;
}

bool Connection::readAndHandle() {
  return false;
}
