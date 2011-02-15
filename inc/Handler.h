#ifndef HANDLER_H
#define HANDLER_H


#include <cstddef>
#include <string>
#include <utility>
#include "HTTPRequest.h"
#include "HTTPResponse.h"
#include "Config.h"

class Handler {
  public:
    static Handler* getInstance();
    std::pair<std::string, int> handle(std::string& request);
  private:
    static Handler* _instance;
    HTTPRequest _request;
    HTTPResponse _response;
    Config _config;


    //Make constructor, copy constructor and assignment operator private
    //to make sure classes don't get instantiated.
    Handler();
    Handler(Handler const&);
    Handler& operator=(Handler const&);
};
#endif /* end of include guard: HANDLER_H */
