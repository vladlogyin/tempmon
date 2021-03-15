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

#include <client.h>
#include <tempmon-cli.h>

std::vector<std::shared_ptr<Client>> clients;

#ifdef TEMPMON_CLI
UIDiv* rootdiv;
#endif

bool running;

void broadcastHandler();

void listenerHandler();

void handleClientDisconnect(std::shared_ptr<Client>);
void handleClientUpdate(std::shared_ptr<Client>);
void handleClientConnect(std::shared_ptr<Client>);
