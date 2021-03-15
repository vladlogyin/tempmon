#include <client.h>

Client::Client(std::string name)
{
  this->name = "no_name";
  this->propertyvalues = std::vector<float>();
  this->propertynames = std::vector<std::string>();
  this->id=0;
}

void Client::StartThread(int fd)
{
  this->fd = fd;
  StartThread();
}


void Client::StartThread()
{
  thread = std::thread(&Client::clientHandler, this);
}

void Client::clientHandler()
{
#ifdef DEBUG
  std::cout << "Spawned thread " << std::this_thread::get_id() << std::endl;
#endif
  
  // Send handshake message
  uint8_t message[] = {'\n','t','e','m','p','m','o','n','a','s','d','f','v'};
  message[12] = VERSION;
  send(this->fd, message, sizeof(message), 0);
  
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
      while((bufsize = read(this->fd, buff, 1))<0);
      switch(buff[0])
      {
        case 0x79: // 4 byte client UID
          bufsize=read(this->fd, buff, 4);
          // Convert byte array back to an int (little endian)
          this->UID=buff[3] + ((uint32_t)buff[2]<<8) + ((uint32_t)buff[1]<<16) + ((uint32_t)buff[0]<<24);
#ifdef DEBUG
          std::cout << std::to_string(buff[0]) << std::to_string(buff[1]) << std::to_string(buff[2]) << std::to_string(buff[3]) << " client UID"<< std::to_string(this->UID)<<std::endl;
#endif
          break;
        case 0x80: // Number of parameters
          bufsize=read(this->fd, buff, 1);
          this->propertycount = buff[0];
          break;
        case 0x81: // Length of parameter name
          bufsize=read(this->fd, buff, 1);
          {
            int namesize = buff[0];
            bufsize=read(this->fd,buff,namesize);
          }
          this->propertynames.emplace_back();
          this->propertyvalues.emplace_back();
          this->propertynames.back().assign((char*)buff, bufsize);
#ifdef DEBUG
          std::cout << this->propertynames.back() << std::endl;
#endif
          break;
        case 0x82: // Clear parameter list
          this->propertycount=0;
          this->propertynames.clear();
          this->propertyvalues.clear();
          break;
        case 0x89:
          done=true;
          break;
      }
    }
    if(!done)
    {
#ifdef DEBUG
      std::cout<< "failed to get description"<<std::endl;
#endif
      
    }
  }
  if(this->UID)
  {
    std::ostringstream oss;
    oss << std::hex << this->UID;
    this->name = "sen"+oss.str();
  }
    
    
  this->connected(shared_from_this());
  linked=true;
  // Set socket to nonblocking
  fcntl(this->fd, F_SETFL, O_NONBLOCK);
  
  
  // Enter client loop
  int retries=0;
  while(linked && retries<10)
  {
    retries++;
    while((bufsize = read(this->fd, buff, this->propertycount * sizeof(float))) > 0)
    {
#ifdef DEBUG
      std::cout<< "read: " << bufsize << std::endl;
#endif
      if((size_t)bufsize == this->propertycount * sizeof(float))
      {
        // Reset timeout loop
        retries = 0;
        for(int prop = 0; prop < this->propertycount; prop++)
        {
          this->propertyvalues[prop] = (float&)buff[prop*sizeof(float)];
#ifdef DEBUG
          std::cout << this->propertynames[prop] << ": " << this->propertyvalues[prop] << std::endl;
#endif
        }
        
          this->updated(shared_from_this());
      }
    }
    
    // Exit if connection closed
    // Wait until buffer has sufficient data
    int peeks=0;
    do
    {
      peeks++;
      if(peeks > 10)
        break;
      //std::cout<<"bufsize="<<bufsize<<std::endl;
      std::this_thread::sleep_for(std::chrono::milliseconds(100));
    }while((bufsize=recv(this->fd, buff, this->propertycount * sizeof(float)+1, MSG_PEEK))<(ssize_t)(this->propertycount * sizeof(float)));
    if(bufsize == 0 || peeks > 10)
      break;
  }
  
  this->disconnected(shared_from_this());

#ifdef DEBUG
  std::cout << "Exited thread " << std::this_thread::get_id() << std::endl;
#endif
}
