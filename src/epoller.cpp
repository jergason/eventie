#include <arpa/inet.h>
#include <stdlib.h>
#include <sys/epoll.h>
#include <string.h>
#include <unistd.h>
#include <iostream>
#include <map>
#include "Handler.h"
#include "Connection.h"
#include "Config.h"

using namespace std;
bool g_debug = false;

int main(int argc, char **argv) {
  struct sockaddr_in server, client;
  socklen_t clientlen = sizeof(client);
  int option, port, s, c;
  int opt;

  // setup default arguments
  port = 3000;

  // process command line options using getopt()
  // see "man 3 getopt"
  while ((option = getopt(argc,argv,"p:d")) != -1) {
    switch (option) {
      case 'p':
        port = atoi(optarg);
        break;
      case 'd':
        g_debug = true;
        break;
      default:
        cout << "server [-p port]" << endl;
        exit(EXIT_FAILURE);
    }
  }

  //Read in configuration file to set some parameters
  Config config;
  config.parse("web.conf");
  string timeout_str = config.parameter("timeout");
  int timeout = atoi(timeout_str.c_str());
  if (g_debug) {
    cout << "in " << __FUNCTION__ << " on line " << __LINE__ << " of file " << __FILE__ << endl;
    cout << "timeout is " << timeout << endl;
  }

  // setup socket address structure
  memset(&server,0,sizeof(server));
  server.sin_family = AF_INET;
  server.sin_port = htons(port);
  server.sin_addr.s_addr = INADDR_ANY;

  // create socket
  s = socket(PF_INET,SOCK_STREAM,0);
  if (!s) {
    perror("socket");
    exit(-1);
  }

  // set socket to immediately reuse port when the application closes
  opt = 1;
  if (setsockopt(s, SOL_SOCKET, SO_REUSEADDR, &opt, sizeof(opt)) < 0) {
    perror("setsockopt");
    exit(-1);
  }

  // call bind to associate the socket with our local address and
  // port
  if (bind(s,(const struct sockaddr *)&server,sizeof(server)) < 0) {
    perror("bind");
    exit(-1);
  }

  // convert the socket to listen for incoming connections
  if (listen(s,SOMAXCONN) < 0) {
    perror("listen");
    exit(-1);
  }

  int epfd = epoll_create(1);

  // add listening socket to poller
  static struct epoll_event ev;
  ev.events = EPOLLIN;
  ev.data.fd = s;
  epoll_ctl(epfd, EPOLL_CTL_ADD, s, &ev);
  map<int, Connection> sockets;

  while (1) {
    // do poll
    // TODO: timeout functionality for sockets
    struct epoll_event* events = NULL;
    //timeout is in seconds, so we turn it into milliseconds here
    int number_of_new_connections = epoll_wait(epfd, events, 1000, timeout *  1000);
    if (number_of_new_connections < 0) {
      perror("epoll");
      exit(EXIT_FAILURE);
    }
    if (number_of_new_connections == 0) {
      //Handle timeout in here?
      //Loop through all sockets
      time_t current_time = time(NULL);
      map<int, Connection>::iterator i = sockets.begin();
      vector<int> to_remove;
      for(;i != sockets.end() i++) {
        if (i->second.shouldTimeout(current_time, timeout)) {
          close(i->first);
          to_remove.push_back(i->first);
          ev.events = EPOLLIN;
          ev.data.fd = fd;
          epoll_ctl(epfd, EPOLL_CTL_DEL, fd, &ev);
        }
      }
      continue;
    }

    // handle sockets that are ready
    for (int i = 0; i < number_of_new_connections; i++) {

      int fd = events[i].data.fd;
      //If there is new data on the server socket, it means someone else is trying to connect to us.
      if (fd == s) {
        c = accept(s, (struct sockaddr *)&client, &clientlen);
        if (c < 0) {
          perror("accept");
          break;
        }

        // add new client to poller
        ev.events = EPOLLIN;
        ev.data.fd = c;
        epoll_ctl(epfd, EPOLL_CTL_ADD, c, &ev);
        // Add client to our map of connections;
        sockets[c] = Connection(c);
      }
      else {
        // handle client
        bool result = sockets[c].readAndHandle();
        if (!result) {
          // socket closed, so remove it from poller and map of connections.
          ev.events = EPOLLIN;
          ev.data.fd = fd;
          epoll_ctl(epfd, EPOLL_CTL_DEL, fd, &ev);
          sockets.erase(c);
          close(fd);
        }
      }
    }
  }
  close(s);
}
