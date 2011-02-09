#include <iostream>
#include <string>

#include "HTTPRequest.h"
#include "HTTPResponse.h"

using namespace std;

int main()
{
      // test HTTP Request parsing
    cout << endl << "*** Test 1 ***" << endl;
    string s = string("GET / HTTP/1.1\r\nHost:www.cnn.com\r\nUser-Agent:downloader\r\nFrom: zappala\r\n\r\n");
    HTTPRequest request;
    request.parse(s);
    cout << request.method() << " " << request.uri() << " "  << request.version() << endl;
    s = request.header(string("Host"));
    cout << "Host: " << s << endl;
    s = request.header(string("User-Agent"));
    cout << "User-Agent: " << s << endl;
    s = request.header(string("From"));
    cout << "From: " << s << endl;
    s = request.header(string("Data"));
    if (s.empty())
        cout << "No Data header" << endl;
    else
        cout << "Data: " << s << endl;

      // test HTTP Request creation
    cout << endl <<  "*** Test 2 ***" << endl;
    request = HTTPRequest();
    request.method("GET");
    request.uri(string("./index.html"));
    request.version("HTTP/1.1");
    request.header("Host","ilab.cs.byu.edu");
    request.header("From","zappala");
    request.header("Cookie",10);
    cout << request.str();

      // test HTTP Response parsing
    cout << endl <<  "*** Test 3 ***" << endl;
    s = string("HTTP/1.1 200 OK\r\nServer:Apache\r\nContent-Length:1000\r\n\r\n");
    HTTPResponse response;
    response.parse(s);
    cout << response.version() << " " << response.code() << " " << response.phrase() << endl;
    s = response.header(string("Server"));
    cout << "Server: " << s << endl;
    s = response.header(string("Content-Length"));
    cout << "Content-Length: " << s << endl;
    s = response.header(string("Data"));
    if (s.empty())
        cout << "No Data header" << endl;
    else
        cout << "Data: " << s << endl;

      // test HTTP Response creation
    cout << endl <<  "*** Test 4 ***" << endl;
    response = HTTPResponse();
    response.version("HTTP/1.1");
    response.code("200");
    response.phrase(string("OK"));
    response.header("Server","Apache");
    response.header("Content-Length",1000);
    cout << response.str();
}
