#include "Handler.h"

using namespace std;

Handler* Handler::_instance = NULL;

Handler* Handler::getInstance() {
  if (!_instance) {
    _instance = new Handler;
  }
  return _instance;
}


string Handler::handle(string& request) {
  //parse the HTTP request
  //
}
