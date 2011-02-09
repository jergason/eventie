#include "Handler.h"

using namespace std;

Handler* Handler::_instance = NULL;

Handler* Handler::getInstance() {
  if (!_instance) {
    _instance = new Handler;
  }
  return _instance;
}

Handler::Handler() {
  _config.parse("web.conf");
}


string Handler::handle(string& request) {
  //parse the HTTP request
  _request.parse(request);

  //We only implement HTTP 1.1, so return an error if it is HTTP 1.0
  if (_request.version() == "HTTP/1.0") {
    return "This server only supports HTTP 1.1.";
  }

  if (_request.method() != "GET") {
    return "This server only supports the HTTP GET verb.";
  }

  //look for host header
  if (_request.header("Host") == "") {
    return "No Host header field!";
  }

  string root_path = _config.host(_request.header("Host"));
  //Make sure the host exists
  if (root_path == "") {
    return "No path for the host " + _request.header("Host");
  }

  //See if the file they requested exists.
  // If not, return a 404
  // If it does exists, build the headers.
  // How does the Connection object know whether to send a file or not?

  return "";
}
