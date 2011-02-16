#ifndef HANDLER_H
#define HANDLER_H


#include <cstddef>
#include <cstring>
#include <string>
#include <utility>
#include <errno.h>
#include <stdio.h>
#include <fcntl.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <unistd.h>
#include <sys/socket.h>
#include <sys/sendfile.h>

#include "HTTPRequest.h"
#include "HTTPResponse.h"
#include "Config.h"
#include "includes.h"

class Handler {
  public:
    static Handler* getInstance();
    bool handle(std::string& request, int sock);
  private:
    static Handler* _instance;
    HTTPRequest _request;
    Config _config;

    bool sendErrorResponseAndHTML(HTTPResponse& response, const char* code, int sock);
    std::string buildErrorHTML(const char* error);
    bool send(std::string& message, int sock);
    bool sendFile(std::string& file_path, int sock, int fd, size_t file_size);
    std::string date(time_t t);
    void setResponseDate(HTTPResponse& response, time_t* time_ptr);

    //Make constructor, copy constructor and assignment operator private
    //to make sure classes don't get instantiated.
    Handler();
    Handler(Handler const&);
    Handler& operator=(Handler const&);
};
#endif /* end of include guard: HANDLER_H */
