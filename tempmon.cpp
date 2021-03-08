#include <tempmon.h>
//TODO: replace exit calls in each thread with a more gentle approach 
// running = false then join all threads and then exit main thread

//#define DEBUG

int main()
{
  std::cout << "tempmon\n";

  running = true;
  
  // Initialize a list to keep track of clients
  // TODO: clean up list once client disconnects
  clients = std::vector<Client>();
  // Reserve enough space so that the vector doesn't have to resize
  clients.reserve(10);
  
  std::thread broadcastThread(broadcastHandler);
  std::thread listenerThread(listenerHandler);
  
  noecho();
  initscr();
  start_color();
  init_pair(0, COLOR_WHITE, COLOR_BLACK);
  init_pair(1, COLOR_YELLOW, COLOR_GREEN);
  init_pair(2, COLOR_CYAN, COLOR_BLUE);
  init_pair(3, COLOR_BLACK, COLOR_WHITE);
  init_pair(4, COLOR_RED, COLOR_MAGENTA);
  
  //UIBar* bar = new UIBar();
  
  rootdiv= new UIDiv(
    {0,0},{60,16},Horizontal,{});
  rootdiv->draw();
  
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
  
  for(Client &c : clients)
  {
    c.thread.join();
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
      clients.push_back(Client("asd"));
      clients.back().connected= handleClientConnect;
      clients.back().updated= handleClientUpdate;
      clients.back().disconnected= handleClientDisconnect;
      
      clients.back().StartThread(new_socket);
    }
    std::this_thread::sleep_for(std::chrono::seconds(2));
  }
#ifdef DEBUG
  std::cout << "Listener thread stopped"<< std::endl;
#endif
}

void clientHandler(Client &client)
{
#ifdef DEBUG
  std::cout << "Spawned thread " << std::this_thread::get_id() << std::endl;
#endif
  
  // Send handshake message
  uint8_t message[] = {'\n','t','e','m','p','m','o','n','a','s','d','f','v'};
  message[12] = VERSION;
  send(client.fd, message, sizeof(message), 0);
  
  // Create rx buffer
  uint8_t buff[64*1024];
  int bufsize;
  
  // Receive client description
  {
    bool done = false;
    for(int i=0;i<20 && !done;i++)
    {
      //TODO make socket non-blocking and add some sort of timeout
      // Read opcode
      while((bufsize = read(client.fd, buff, 1))<0);
      switch(buff[0])
      {
        case 0x80: // Number of parameters
          bufsize=read(client.fd, buff, 1);      
          client.propertycount = buff[0];
          break;
        case 0x81: // Length of parameter name
          bufsize=read(client.fd, buff, 1);
          {
            int namesize = buff[0];
            bufsize=read(client.fd,buff,namesize);
          }
          client.propertynames.emplace_back();
          client.propertyvalues.emplace_back();
          client.propertynames.back().assign((char*)buff, bufsize);
#ifdef DEBUG
          std::cout << client.propertynames.back() << std::endl;
#endif
          break;
        case 0x82: // Clear parameter list
          client.propertycount=0;
          client.propertynames.clear();
          client.propertyvalues.clear();
          break;
        case 0x89:
          done=true;
          break;
      }
    }
    if(!done)
    {
#ifdef DEBUG
      std::cout<< "failed to get description";
#endif
      
    }
  }
  
  client.connected(&client);
  
  // Set socket to nonblocking
  fcntl(client.fd, F_SETFL, O_NONBLOCK);
  
  
  // Enter client loop
  int retries=0;
  while(running && retries<10)
  {
    retries++;
    while((bufsize = read(client.fd, buff, client.propertycount * sizeof(float))) > 0)
    {
#ifdef DEBUG
      std::cout<< "read: " << bufsize << std::endl;
#endif
      if((size_t)bufsize == client.propertycount * sizeof(float))
      {
        // Reset timeout loop
        retries = 0;
        for(int prop = 0; prop < client.propertycount; prop++)
        {
          client.propertyvalues[prop] = (float&)buff[prop*sizeof(float)];
#ifdef DEBUG
          std::cout << client.propertynames[prop] << ": " << client.propertyvalues[prop] << std::endl;
#endif
        }
        
          client.updated(&client);
      }
    }
    
    // Exit if connection closed
    if(bufsize == 0)
      break;
    // Wait until buffer has sufficient data
    while(((bufsize=recv(client.fd, buff, client.propertycount * sizeof(float)+1,MSG_PEEK))<(ssize_t)(client.propertycount * sizeof(float)))&&bufsize!=0)
      std::this_thread::sleep_for(std::chrono::milliseconds(100));
     
  }
  
  client.disconnected(&client);

#ifdef DEBUG
  std::cout << "Exited thread " << std::this_thread::get_id() << std::endl;
#endif
}

Client::Client(std::string name)
{
  this->name = "sens"+std::to_string(nextid);
  this->propertyvalues = std::vector<float>();
  this->propertynames = std::vector<std::string>();
  this->id = nextid++;
}

void Client::StartThread(int fd)
{
  this->fd = fd;
  StartThread();
  //return this->StartThread();
}


void Client::StartThread()
{
  thread = std::thread(clientHandler, std::ref(*this));
  //return thread;
}

void drawBar(uint16_t x, uint16_t y, uint16_t l)
{
  
}

UILabel* label;
void handleClientDisconnect(Client* c)
{
  // look for div belonging to client
  for(int i=0; i < rootdiv->elements.size(); i++)
  {
    if(((UILabel*)(((UIDiv*)(rootdiv->elements[i]))->elements[0]))->text==c->name)
    {
      rootdiv->elements.erase(rootdiv->elements.begin()+i);
      break;
    }
  }
}
bool updateturn=false;
void handleClientUpdate(Client* c)
{
  label->text = std::to_string(c->propertyvalues[0]);
  label->color = (updateturn = !updateturn)?1:2;
  rootdiv->draw();
  refresh();
}
void handleClientConnect(Client* c)
{
  rootdiv->elements.push_back(new UIDiv(Vertical,{new UILabel(c->name,1),new UIFrame(new UIDiv(Horizontal,{ new UILabel(c->propertynames[0] + ": ",1), (label=new UILabel(std::to_string(c->propertyvalues[0]),1))}), Fill)}));
  updatedisplay=true;
  rootdiv->draw();
  refresh();
}
