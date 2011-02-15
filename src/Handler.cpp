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


pair<string, int> Handler::handle(string& request) {
  //parse the HTTP request
  _request.parse(request);

  //We only implement HTTP 1.1, so return an error if it is HTTP 1.0
  if (_request.version() == "HTTP/1.0") {
    throw "This server only supports HTTP 1.1.";
  }

  if (_request.method() != "GET") {
    throw "This server only supports the HTTP GET verb.";
  }

  //look for host header
  if (_request.header("Host") == "") {
    throw "No Host header field!";
  }

  string root_path = _config.host(_request.header("Host"));
  //Make sure the host exists
  if (root_path == "") {
    throw "No path for the host " + _request.header("Host");
  }
  //@TODO: take care of special case where "/" is the path. Serve "/index.html"

  //See if the file they requested exists.
  // If not, return a 404
  // If it does exists, build the headers.
  //    Get size, last_modified, check for permissions, check filetype, ect
  //    If type exists, then use that as the MIME type, otherwise use text/plain

  //What do we return back to the connection object? A response object with an optional path
  //for a file to send.
  pair<string, int> res("hurp", -1);
  return res;
}
