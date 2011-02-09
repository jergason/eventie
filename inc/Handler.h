#ifndef HANDLER_H
#define HANDLER_H


#include <cstddef>
#include <string>

class Handler {
  public:
    static Handler* getInstance();
    std::string handle(std::string& request);
  private:
    static Handler* _instance;



    //Make constructor, copy constructor and assignment operator private
    //to make sure classes don't get instantiated.
    Handler(){};
    Handler(Handler const&) {};
    Handler& operator=(Handler const&) {};
};
#endif /* end of include guard: HANDLER_H */
