#ifndef CLIENT_H
#define CLIENT_H

#include <string>
#include <sstream>
#include <thread>
#include <vector>
#include <functional>

#include <fcntl.h>
#include <unistd.h>
#include <sys/socket.h>
#include <stdlib.h>
#include <netinet/in.h>
#include <arpa/inet.h>

#define VERSION 1
#define BCPORT 9988
#define TCPPORT 9989

class Client : public std::enable_shared_from_this<Client>
{
public:
  int id;
  uint32_t UID=0;
  std::string name;
  
  bool linked=false;

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
  std::function<void(std::shared_ptr<Client>)> connected;
  std::function<void(std::shared_ptr<Client>)> updated;
  std::function<void(std::shared_ptr<Client>)> disconnected;

private:
  void clientHandler();

};
#endif
