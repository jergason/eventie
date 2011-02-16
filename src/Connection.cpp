#include "Connection.h"

using namespace std;

string Connection::SENTINEL = "\r\n\r\n";

Connection::Connection(int sock) {
  _sock = sock;
  _buffer = "";
  BUFFER_SIZE = 1024;
  last_used = time(NULL);
}

Connection::Connection() {
  _sock = -1;
  BUFFER_SIZE = 1024;
  last_used = time(NULL);
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
  if (g_debug) {
    cout << "In " << __FILE__ << " in " << __FUNCTION__ << " on "
    << __LINE__ << endl;
  }
  last_used = time(NULL);
  char buf[BUFFER_SIZE];
  memset(buf, 0, BUFFER_SIZE);
  // Loop so we can call recv again if interrupted by the OS.
  while (1) {
    if (g_debug) {
      cout << "In " << __FILE__ << " in " << __FUNCTION__ << " on "
      << __LINE__ << endl;
      cout << "looping in readAndHandle() on socket " << _sock << endl;
    }
    //Is there something blocking us in recv?
    int bytes_received = recv(_sock, buf, BUFFER_SIZE, MSG_DONTWAIT);
    if (g_debug) {
      cout << "In " << __FILE__ << " in " << __FUNCTION__ << " on "
      << __LINE__ << endl;
      cout << "after recv and recieved " << bytes_received << endl; 
    }
    if (bytes_received < 0) {
      if (errno == EINTR) {
        //call to recv was interruted by OS
        continue;
      }
      else {
        //some kind of error
        perror("error reading from client socket");
        if (errno == EAGAIN || errno == EWOULDBLOCK) {
          if (g_debug) {
            cout << "In " << __FILE__ << " in " << __FUNCTION__ << " on "
            << __LINE__ << endl;
            cout << "would block? whaaa?" << endl;
          }
          return true;
        }
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
  int sentinel_position = _buffer.find(Connection::SENTINEL);
  if (sentinel_position != static_cast<int>(string::npos)) {
    request = _buffer.substr(0, sentinel_position + 4);
    _buffer = _buffer.substr(sentinel_position + 4);
  }
  
  if (request != "") {
    if (g_debug) {
      cout << "In " << __FILE__ << " in " << __FUNCTION__ << " on "
      << __LINE__ << endl;
      cout << "found something in the buffer" << endl;
    }
    //If buffer includes a sentinel, call Handler.getInstance->handle(buffer_chunk_until_sentinel, socket)
    bool to_return = Handler::getInstance()->handle(request, _sock);
    // Make sure we set timeout after sending response, in case
    // we are sending a large file that takes a while to send.
    last_used = time(NULL);
    return to_return;
  }
  return true;
}

/**
 * Determine if the socket should timeout.
 */
bool Connection::shouldTimeout(int timeout) {
  time_t now = time(NULL);
  return ((now - last_used) > timeout); 
}
