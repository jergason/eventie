#include "Config.h"

using namespace std;

//
// Config: public methods
//

Config::Config()
{
}

Config::~Config()
{
}

void Config::parse(const char* filename)
{
    host_.clear();
    media_.clear();
    parameter_.clear();

    string line;

      // open file
    ifstream file(filename);
    if (!file.is_open())
        return;

      // parse file
    while (!file.eof()) {
        getline(file,line);
        parseLine(line);
    }

      // close file
    file.close();
}

string&
Config::host(string name)
{
    return host_[name];
}

string&
Config::media(string name)
{
    return media_[name];
}

string&
Config::parameter(string name)
{
    return parameter_[name];
}

//
// Config: private methods
//

void Config::parseLine(std::string &line)
{
    vector<string> tokens = tokenizer_.parse(line);
    if (tokens.size() < 3)
        return;
    if (tokens.at(0) == "host")
        host_[tokens.at(1)] = tokens.at(2);
    else if (tokens.at(0) == "media")
        media_[tokens.at(1)] = tokens.at(2);
    else if (tokens.at(0) == "parameter")
        parameter_[tokens.at(1)] = tokens.at(2);
}

bool Config::Test(ostream& os) {
  bool success = true;
  // parse Configuration
  Config cfg;
  cfg.parse("test/web.conf");

  cout << "Root for carmelo: " << cfg.host("carmelo.cs.byu.edu") << endl;
  cout << "Root for localhost: " << cfg.host("localhost") << endl;

  cout << "Media for txt: " << cfg.media("txt") << endl;
  cout << "Media for html: " << cfg.media("html") << endl;
  cout << "Media for jpg: " << cfg.media("jpg") << endl;
  cout << "Media for gif: " << cfg.media("gif") << endl;
  cout << "Media for png: " << cfg.media("png") << endl;
  cout << "Media for pdf: " << cfg.media("pdf") << endl;
  cout << "Media for py: " << cfg.media("py") << endl;

  cout << "Timeout parameter in seconds: " << cfg.parameter("timeout") << endl;
  return success;
}
