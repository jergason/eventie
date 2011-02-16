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

/**
 * Parse and incoming HTTP request and construct a valid response.
 * Send file if there is a file to send, or send the HTML error response if not.
 *
 * Currently the server only implements the GET method, so any other
 * method will result in a 501 Not Implemented error.
 */
bool Handler::handle(string& request, int sock) {
  _request.parse(request);
  HTTPResponse _response = HTTPResponse();
  _response.version("HTTP/1.1");
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

  string uri = _request.uri();
  // Take care of special case where "/" is the path. Serve "/index.html"
  if (uri == "/") {
    uri = "/index.html";
  }


  string file_path = root_path + uri;
  // See if the file they requested exists.
  int fd = open(file_path.c_str(), O_RDONLY);
  if (fd == -1) {
    if (errno == EACCES) {
      // Don't have permissions to access file, 403 error.
      return sendErrorResponseAndHTML(_response, "403 Forbidden", sock);
    }
    else if (errno == ENOENT) {
      // File does not exist. 404 error.
      return sendErrorResponseAndHTML(_response, "404 Not Found", sock);
    }
    else {
      return sendErrorResponseAndHTML(_response, "500 Internal Server Error", sock);
    }
  }
  
  // File exists, build the headers.
  struct stat stats;
  int result = fstat(fd, &stats);
  if (result == -1) {
    if (g_debug) {
      cout << "Error in " << __FUNCTION__ << " on line " << __LINE__ << " of file " << __FILE__ << endl;
    }
    return sendErrorResponseAndHTML(_response, "500 Internal Server Error", sock);
  }

  //    Get size, last_modified, check for permissions, check filetype, ect
  //    If type exists, then use that as the MIME type, otherwise use text/plain

  //What do we return back to the connection object? A response object with an optional path
  //for a file to send.
  return true;
}

/**
 * Send an HTTP response, and an HTML error string, to sock.
 */
bool Handler::sendErrorResponseAndHTML(HTTPResponse& response, const char* code, int sock) {
  string html = buildErrorHTML(code);
  response.code(code);
  //@TODO: set Date and Server header
  response.header("Content-Type", "text/html");
  response.header("Content-Length", strlen(html.c_str()));
  //No idea why it was throwing an error when I har this inline
  string hurp = response.str();
  if (send(hurp, sock)) {
    return send(html, sock);
  }
  else {
    return false;
  }
}

/**
 * Construct the HTML error string to send back along with the HTTP response.
 */
string Handler::buildErrorHTML(const char* error) {
  string err = error;
  string res = "<html>\n\t<head>\n\t\t<title>" + err + "</title>\n\t</head>";
  res += "\n\t<body>\n\t\t<h1>Error: " + err + "</h1>\n\t\t<p>Sorry, but it looks like there was an error in serving your request.</p>";
  res += "\n\t</body>\n</html>";
  return res;
}

/**
 * Send message to the socket sock. Return true if the send was successful, or false the 
 * socket was closed or there was some kind of error.
 */
bool Handler::send(string& message, int sock) {
  const char *out = message.c_str();
  int chars_sent = 0;
  int total_chars_sent = 0;
  while (total_chars_sent < static_cast<int>(message.length())) {
    chars_sent = ::send(sock, (const void *)out, message.length() - total_chars_sent, 0);
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
  return true;
}

// bool Handler::send(const char* message, int sock) {
//   string mess = message;
//   return send(mess, sock);
// }
