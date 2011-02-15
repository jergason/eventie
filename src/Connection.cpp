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
  //If buffer includes a sentinel, call Handler.getInstance->handle(buffer_chunk_until_sentinel)
  pair<string, int> response;
  if (request != "") {
    response = Handler::getInstance()->handle(request);
  }
  else {
    throw "Error in parsing request from buffer.";
  }
  //Attempt to send the first part of the response.
  if (!this->send(response.first)) {
      return false;
  }

  //If there is a valid file to accompany the response, then call send_file on it
  if (response.second != -1) {
    //send the file, and return the result

    struct stat stats;
    int res = fstat(response.second, &stats);

    sendfile(_sock, response.second, NULL, stats.st_size);
    //loop to send the file
    // int total_sent = 0;
    // int bytes_sent = 0;
    // int bytes_to_send = stats.st_size;
    // off_t file_offset = 0;
    // while (total_sent < stats.st_size) {
    //   bytes_sent = sendfile(_sock, response.second, &file_offset, bytes_to_send);
    //   total_sent += bytes_sent;
    //   bytes_to_send -= bytes_sent;
    // }
    return true;
  }
  return false;
}

bool Connection::send(string &message) {
  const char *out = message.c_str();
  int chars_sent = 0;
  int total_chars_sent = 0;
  while (total_chars_sent < static_cast<int>(message.length())) {
    chars_sent = ::send(_sock, (const void *)out, message.length() - total_chars_sent, 0);
    if (chars_sent < 0) {
      if (errno == EINTR) {
        continue;
      }
      else {
        //some kind of error
        return false;
      }
    }
    else if (errno == 0) {
      //socket closed
      return false;
    }
    out += chars_sent;
    total_chars_sent += chars_sent;
  }
  return 0;
}

