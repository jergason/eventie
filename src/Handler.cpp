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
  _response.header("Server", "Eventie");
  //We only implement HTTP 1.1, so return an error if it is HTTP 1.0
  if (_request.version() == "HTTP/1.0") {
    if (g_debug) {
      cout << "Error in " << __FUNCTION__ << " on line " << __LINE__ << " of file " << __FILE__ << endl;
      cout << "request for HTTP/1.0" << endl;
    }
    return sendErrorResponseAndHTML(_response, "501 Not Implemented", sock);
  }

  if (_request.method() != "GET") {
    if (g_debug) {
      cout << "Error in " << __FUNCTION__ << " on line " << __LINE__ << " of file " << __FILE__ << endl;
      cout << "Request for method besides GET: " << _request.method() << endl;
    }
    return sendErrorResponseAndHTML(_response, "501 Not Implemented", sock);
  }

  //look for host header
  if (_request.header("Host") == "") {

    if (g_debug) {
      cout << "Error in " << __FUNCTION__ << " on line " << __LINE__ << " of file " << __FILE__ << endl;
      cout << "host header is empty" << endl;
    }
    return sendErrorResponseAndHTML(_response, "400 Bad Request", sock);
  }

  //Strip off anything including and after colon
  string root_path = _config.host(_request.header("Host"));
  //Make sure the host exists
  if (root_path == "") {
    if (g_debug) {
      cout << "Error in " << __FUNCTION__ << " on line " << __LINE__ << " of file " << __FILE__ << endl;
      cout << "root_path is empty" << endl;
    }
    return sendErrorResponseAndHTML(_response, "400 Bad Request", sock);
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
      if (g_debug) {
        cout << "Error in " << __FUNCTION__ << " on line " << __LINE__ << " of file " << __FILE__ << endl;
        cout << "403 error for " << file_path << endl;
      }
      return sendErrorResponseAndHTML(_response, "403 Forbidden", sock);
    }
    else if (errno == ENOENT) {
      // File does not exist. 404 error.
      if (g_debug) {
        cout << "Error in " << __FUNCTION__ << " on line " << __LINE__ << " of file " << __FILE__ << endl;
        cout << "404 error for " << file_path << endl;
      }
      return sendErrorResponseAndHTML(_response, "404 Not Found", sock);
    }
    else {
      if (g_debug) {
        cout << "Error in " << __FUNCTION__ << " on line " << __LINE__ << " of file " << __FILE__ << endl;
        cout << "500 error of some kind" << endl;
      }
      return sendErrorResponseAndHTML(_response, "500 Internal Server Error", sock);
    }
  }

  // File exists, build the headers.
  struct stat stats;
  int result = fstat(fd, &stats);
  if (result == -1) {
    if (g_debug) {
      cout << "Error in " << __FUNCTION__ << " on line " << __LINE__ << " of file " << __FILE__ << endl;
      cout << "Error in fstat" << endl;
    }
    return sendErrorResponseAndHTML(_response, "500 Internal Server Error", sock);
  }

  off_t file_size = stats.st_size;
  time_t last_modified = stats.st_mtime;
  
  // Check for file extension to correctly set content type
  int extension_beginning = uri.find_last_of(".");
  string extension = "";
  string content_type = "text/plain";
  if (extension_beginning != string::npos) {
    extension = uri.substr(extension_beginning + 1);
    string content_type = _config.media(extension);
    if (content_type == "") {
      content_type = "text/plain";
    }
  }
  
  _response.header("Content-Type", content_type);
  _response.header("Content-Length", file_size);
  _response.header("Last-Modified", last_modified);
  string hurp = _response.str();
  if (g_debug) {
    cout << "In " << __FILE__ << " in " << __FUNCTION__ << " on "
    << __LINE__ << endl;
    cout << "sending response: " << _response.str() << endl;
  }
  
  if (send(hurp, sock)) {
    return sendFile(file_path, sock, fd, file_size);
  }
  else {
    return false;
  }
}

bool Handler::sendFile(string& file_path, int sock, int fd, size_t file_size) {
  //loop to send the file
  if (g_debug) {
    cout << "In sendFile on line " << __LINE__ << " of file " << __FILE__
    << "and attempting to send a file" << endl;
  }
  int total_sent = 0;
  int bytes_sent = 0;
  int bytes_to_send = file_size;
  off_t file_offset = 0;
  while (total_sent < file_size) {
    bytes_sent = sendfile(sock, fd, &file_offset, bytes_to_send);
    total_sent += bytes_sent;
    bytes_to_send -= bytes_sent;
    file_offset += bytes_sent;
  }
  return true;
}

/**
 * Send an HTTP response, and an HTML error string, to sock.
 */
bool Handler::sendErrorResponseAndHTML(HTTPResponse& response, const char* code, int sock) {
  string html = buildErrorHTML(code);
  response.code(code);
  setResponseDate(response, NULL);
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
    else if (chars_sent == 0) {
      //socket closed
      return false;
    }
    out += chars_sent;
    total_chars_sent += chars_sent;
  }
  return true;
}

void Handler::setResponseDate(HTTPResponse& response, time_t* time_ptr) {
  string date_str = date(time(time_ptr));
  response.header("Date", date_str);
}


string Handler::date(time_t t) {
  struct tm *gmt;
  char  buf [200];
  memset(buf,0,200);
  gmt = gmtime(&t);
  if (gmt == NULL)
    return "";
  if (strftime(buf, sizeof(buf), "%a, %d %b %Y %H:%M:%S GMT", gmt) == 0)
    return "";
  return string(buf);
}
