#include <iostream>
#include <thread>
#include <chrono>
#include <algorithm>
#include <vector>
#include <string>
#include <utility>
#include <functional>

#include <cstring>
#include <ctime>

#include <fcntl.h>
#include <unistd.h>
#include <sys/socket.h>
#include <stdlib.h>
#include <netinet/in.h>
#include <arpa/inet.h>

#include <curses.h>
#include <menu.h>

#include <tempui.h>

#define VERSION 1
#define BCPORT 9988
#define TCPPORT 9989

int nextid=0;


class Client {
public:
  int id;
  std::string name;
  
  int fd;
  std::thread thread;
  
  //std::chrono::time_point lastupdate;
  int propertycount;
  std::vector<std::string> propertynames;
  std::vector<float> propertyvalues; 
  
  Client(std::string);
  
  // int filedescriptor
  void StartThread(int);
  void StartThread();
  
  // Client handles
  std::function<void(Client*)> connected;
  std::function<void(Client*)> updated;
  std::function<void(Client*)> disconnected;
  
  
//private:
  //static int nextid=0;
};

std::vector<Client> clients;

UIDiv* rootdiv;


bool running;

void broadcastHandler();

void listenerHandler();

void clientHandler(Client&);


bool updatedisplay;
void handleClientDisconnect(Client*);
void handleClientUpdate(Client*);
void handleClientConnect(Client*);
