#include "Connection.h"

using namespace std;

string Connection::SENTINEL = "\r\r\n\n";

Connection::Connection(int sock) {
  _sock = sock;
  _buffer = "";
  BUFFER_SIZE = 1024;
}

Connection::Connection() {
  _sock = -1;
  BUFFER_SIZE = 1024;
}

void Connection::setSocket(int sock) {
  _sock = sock;
}

/**
 * Call read once on the client socket. Parse the buffer for an
 * HTTP requset. If one is found, pass it off to the handler and
 * send the response to the client that the handler returns.
 *
 * If no response is found, but the socket is still open,
 * then return true. If there is an error or the socket is
 * closed by the client, return false.
 */
bool Connection::readAndHandle() {
  char buf[BUFFER_SIZE];
  memset(buf, 0, BUFFER_SIZE);
  //Loop so we can call recv again if interrupted by the OS.
  while (1) {
    int bytes_received = recv(_sock, buf, BUFFER_SIZE, 0);
    if (bytes_received < 0) {
      if (errno == EINTR) {
        //call to recv was interruted by OS
        continue;
      }
      else {
        //some kind of error
        perror("error reading from client socket");
        return false;
      }
    }
    else if (bytes_received == 0) {
      //connection closed by client, so we return false;
      return false;
    }
    else {
      _buffer += buf;
      break;
    }
  }

  string request = "";
  //Check buffer for a sentinel.
  int sentinel_position = _buffer.find(SENTINEL);
  if (sentinel_position != static_cast<int>(string::npos)) {
    request = _buffer.substr(0, sentinel_position + 1);
    _buffer = _buffer.substr(sentinel_position + 1);
  }
  //If buffer includes a sentinel, call Handler.getInstance->handle(buffer_chunk_until_sentinel, socket)
  return Handler::getInstance()->handle(request, _sock);
}
