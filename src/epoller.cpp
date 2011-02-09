#include <arpa/inet.h>
#include <stdlib.h>
//#include <sys/epoll.h>
#include <unistd.h>
#include <iostream>
#include "Handler.h"

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

  //int epfd = epoll_create(1);

  //// add listening socket to poller
  //static struct epoll_event ev;
  //ev.events = EPOLLIN;
  //ev.data.fd = s;
  //epoll_ctl(epfd, EPOLL_CTL_ADD, s, &ev);

  //while (1) {
    //// do poll
    //struct epoll_event* events = NULL;
    //int number_of_new_connections = epoll_wait(epfd, events, 1000, 1);
    //if (number_of_new_connections < 0) {
      //perror("epoll");
      //exit(EXIT_FAILURE);
    //}
    //if (number_of_new_connections == 0) {
      //continue;
    //}

    //// handle sockets that are ready
    //for (int i = 0; i < number_of_new_connections; i++) {

      //int fd = events[i].data.fd;
      ////If there is new data on the server socket, it means someone else is trying to connect to us.
      //if (fd == s) {
        //c = accept(s, (struct sockaddr *)&client, &clientlen);
        //if (c < 0) {
          //perror("accept");
          //break;
        //}

        //// add new client to poller
        //ev.events = EPOLLIN;
        //ev.data.fd = c;
        //epoll_ctl(epfd, EPOLL_CTL_ADD, c, &ev);
      //}
      //else {
        //// handle client
        //bool result = h.handle(fd);
        //if (!result) {
          //// socket closed, so remove it from poller
          //ev.events = EPOLLIN;
          //ev.data.fd = fd;
          //epoll_ctl(epfd, EPOLL_CTL_DEL, fd, &ev);
          //close(fd);
        //}
      //}
    //}
  //}
  //close(s);
}
