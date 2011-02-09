#include "handler.h"

using namespace std;

Handler* Handler::getInstance() {
  if (!_instance) {
    _instance = new Handler;
  }
  return _instance;
}
