#include <tempmon.h>
//TODO: replace exit calls in each thread with a more gentle approach 
// running = false then join all threads and then exit main thread

//#define DEBUG

int main()
{
  std::cout << "tempmon" << std::endl;

  running = true;
  
  // Initialize a list to keep track of clients
  // TODO: clean up list once client disconnects
  clients = std::vector<std::shared_ptr<Client>>();
  // Reserve enough space so that the vector doesn't have to resize
  clients.reserve(10);
  
  std::thread broadcastThread(broadcastHandler);
  std::thread listenerThread(listenerHandler);
  
#ifdef TEMPMON_CLI
  // Initialize screen and color pairs
  noecho();
  initscr();
  start_color();
  init_pair(0, COLOR_WHITE, COLOR_BLACK);
  init_pair(1, COLOR_YELLOW, COLOR_GREEN);
  init_pair(2, COLOR_CYAN, COLOR_BLUE);
  init_pair(3, COLOR_BLACK, COLOR_WHITE);
  init_pair(4, COLOR_RED, COLOR_MAGENTA);
  
  //UIBar* bar = new UIBar();
  
  // Get terminal size
  int width = 10;
  int height = 10;
  getmaxyx(stdscr, height, width);
  
  // Initialize root UI element
  rootdiv = new UIDiv(
    {0,0},{width,height},Horizontal,{});
  rootdiv->draw();
#endif
  /*printw("Hello World");
  noecho();
  for(int i=0;i<10;i++)
  {
    mvaddch(0,i,'#');
    refresh();
    std::this_thread::sleep_for(std::chrono::milliseconds(500));
  }*/
  refresh();
  
  //while(running)
  //{
    
  //}
  
  while(getchar()!='e')
  {
    
  }
  
  
#ifdef DEBUG
  std::cout << "Stopping"<< std::endl;
#endif
  running = false;
  broadcastThread.join();
  listenerThread.join();
  
  // Join all client threads
  
  for(std::shared_ptr<Client> c : clients)
  {
    c->thread.join();
  }
  
  endwin();
  
  return 0;
}




void broadcastHandler()
{
#ifdef DEBUG
  std::cout << "Started broadcast thread"<< std::endl;
#endif
  
  uint8_t message[] = {'\n','t','e','m','p','m','o','n','a','d','d','r','v'};
  message[12] = VERSION;
  
  sockaddr_in addr;
  int fd;
  if((fd = socket(AF_INET, SOCK_DGRAM, 0)) < 0)
  {
    perror("socket");
    exit(1);
  }
  int opt = 1;
  if(setsockopt(fd, SOL_SOCKET, SO_BROADCAST, &opt, sizeof(opt)))
  {
    perror("setsockopt");

    exit(1);
  }
  // Set up destination address
  addr.sin_family = AF_INET;
  addr.sin_addr.s_addr = inet_addr("192.168.2.255");
  addr.sin_port=htons(BCPORT);
  
  while(running)
  {
    if(sendto(fd, message, sizeof(message), 0, (sockaddr*)&addr, sizeof(addr)) < 0)
    {
      perror("sendto");
      exit(1);
    }
    
    for(int i=0;i<2&&running;i++)
      std::this_thread::sleep_for(std::chrono::seconds(2));
  }
#ifdef DEBUG
  std::cout << "Broadcast thread stopped"<< std::endl;
#endif
}

void listenerHandler()
{
  
  int listenerfd;
  if((listenerfd  = socket(AF_INET, SOCK_STREAM, 0)) == 0)
  {
    perror("socket");
    exit(1);
  }
  int opt = 1;
  if (setsockopt(listenerfd, SOL_SOCKET, SO_REUSEADDR | SO_REUSEPORT, &opt, sizeof(opt))) 
  { 
    perror("setsockopt"); 
    exit(EXIT_FAILURE); 
  } 
  // Setup listening address
  sockaddr_in addr;
  addr.sin_family = AF_INET;
  addr.sin_addr.s_addr = INADDR_ANY;
  addr.sin_port=htons(TCPPORT);
  
  // Try to attach socket to TCPPORT
  if(bind(listenerfd, (sockaddr*)&addr, sizeof(addr)) < 0)
  {
    perror("bind");
    exit(1);
  }
  
  // Set socket to listen for up to 5 incoming connections
  if (listen(listenerfd, 5) < 0) 
  { 
    perror("listen"); 
    exit(1); 
  }
  
  // Set listening socket ot nonblocking mode so that we can stop at any time
  fcntl(listenerfd, F_SETFL, O_NONBLOCK);
  
#ifdef DEBUG
  std::cout << "Started listener thread"<< std::endl;
#endif
  
  int addrlen = sizeof(addr);
  
  while(running)
  {
    // Check for incoming connections
    int new_socket;
    while((new_socket = accept(listenerfd, (sockaddr*)&addr, (socklen_t*)&addrlen)) >= 0)
    {
      //TODO add some sort of authentification or handshake
      
#ifdef DEBUG
      std::cout << "Adding client"<< std::endl;
#endif
      
      // Accept incoming connections
      clients.push_back(std::make_shared<Client>("asd"));
      clients.back()->connected= handleClientConnect;
      clients.back()->updated= handleClientUpdate;
      clients.back()->disconnected= handleClientDisconnect;
      
      clients.back()->StartThread(new_socket);
    }
    std::this_thread::sleep_for(std::chrono::seconds(2));
  }
#ifdef DEBUG
  std::cout << "Listener thread stopped"<< std::endl;
#endif
}


void drawBar(uint16_t x, uint16_t y, uint16_t l)
{
  
}

#ifdef TEMPMON_CLI
//TODO this only updates the last connected client
// probably a hashtable with label values keyed by client UIDs
UILabel* label;
#endif
void handleClientDisconnect(std::shared_ptr<Client> c)
{
  //std::cout<<"disconnecting client"<<std::endl;
#ifdef TEMPMON_CLI
  // look for div belonging to client
  for(int i=0; i < rootdiv->elements.size(); i++)
  {
    //TODO broken
    if(rootdiv->elements[i]->id==c->UID)
    {
        //std::cout<<"client element found"<<std::endl;
      //delete rootdiv->elements[i]
      rootdiv->elements.erase(rootdiv->elements.begin()+i);
      break;
    }
  }
#endif
}
bool updateturn=false;
void handleClientUpdate(std::shared_ptr<Client> c)
{
#ifdef TEMPMON_CLI
  label->settext(std::to_string(c->propertyvalues[0]));
  label->color = (updateturn = !updateturn)?1:2;
  
  rootdiv->redraw=true;
  rootdiv->draw();
  refresh();
#endif
}
void handleClientConnect(std::shared_ptr<Client> c)
{
#ifdef TEMPMON_CLI
  rootdiv->elements.push_back(new UIDiv(Vertical,{new UILabel(std::to_string(c->UID),1),new UIFrame(new UIDiv(Horizontal,{ new UILabel(c->propertynames[0] + ": ",1), (label=new UILabel(std::to_string(c->propertyvalues[0]),1))}), Fill)}));
  rootdiv->elements.back()->id = c->UID;
  rootdiv->redraw=true;
  rootdiv->draw();
  refresh();
#endif
}
