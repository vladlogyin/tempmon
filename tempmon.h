#include <iostream>
#include <thread>
#include <chrono>
#include <algorithm>
#include <vector>
#include <string>
#include <sstream>
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

#ifdef TEMPMON_GTK
#include <tempmon-gtk.h>
#endif

#ifdef TEMPMON_CLI
#include <tempmon-cli.h>
#endif

#define VERSION 1
#define BCPORT 9988
#define TCPPORT 9989

int nextid=0;


class Client {
public:
  int id;
  uint32_t UID=0;
  std::string name;
  
  int fd;
  std::thread thread;
  
  //std::chrono::time_point lastupdate;
  int propertycount;
  std::vector<std::string> propertynames;
  std::vector<float> propertyvalues; 
  
  Client(std::string);
  
  // int filedescriptor
  void StartThread(int, std::shared_ptr<Client>);
  void StartThread(std::shared_ptr<Client>);
  
  // Client handles
  std::function<void(std::shared_ptr<Client>)> connected;
  std::function<void(std::shared_ptr<Client>)> updated;
  std::function<void(std::shared_ptr<Client>)> disconnected;
  
  
//private:
  //static int nextid=0;
};

std::vector<std::shared_ptr<Client>> clients;

#ifdef TEMPMON_CLI
UIDiv* rootdiv;
#endif

bool running;

void broadcastHandler();

void listenerHandler();

void clientHandler(std::shared_ptr<Client>);


bool updatedisplay;
void handleClientDisconnect(std::shared_ptr<Client>);
void handleClientUpdate(std::shared_ptr<Client>);
void handleClientConnect(std::shared_ptr<Client>);
