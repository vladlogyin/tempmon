#include <tempui.h>

UIElement::UIElement(std::pair<uint16_t,uint16_t> position, std::pair<uint16_t,uint16_t> size)
{
  //TODO reconsider this constructor
  //TODO solve naming inconsistencies pos = position
  // both position and size arguments are pretty useless
  this->color = 0; // not sure if this is the "default" color pair
  this->pos = position;
  this->size = size;
  this->sizing = Fill;
}

UIElement::UIElement(int col) //filler constructor
{
  this->color = col;
  this->sizing = Fill;
}

UIBox::UIBox(int col) : UIElement(col){}

void UIBox::draw()
{
  if(redraw)
  {
    attron(COLOR_PAIR(color));
    for(uint16_t y = pos.second; y < pos.second+size.second;y++)
    {
      for(uint16_t x = pos.first; x < pos.first+size.first;x++)
      {
        if(x==pos.first||x==pos.first+size.first-1||y==pos.second||y==pos.second+size.second-1)
        {
          mvaddch(y,x,'#');
        }
        else
        {
        mvaddch(y,x,' '); 
        }
      }
    }
    attroff(COLOR_PAIR(color));
    redraw = false;
  }
}

UIFrame::UIFrame(UIElement* e,int col) : UIElement(col)
{
  this->element = e;
}

void UIFrame::draw()
{
  if(redraw)
  {
    attron(COLOR_PAIR(color));
    for(uint16_t y = pos.second; y < pos.second+size.second;y++)
    {
      for(uint16_t x = pos.first; x < pos.first+size.first;x++)
      {
        if(x==pos.first||x==pos.first+size.first-1||y==pos.second||y==pos.second+size.second-1)
        {
          mvaddch(y,x,'#');
        }
        else
        {
        mvaddch(y,x,' '); 
        }
      }
    }
    attroff(COLOR_PAIR(color));
    this->element->pos={pos.first+1,pos.second+1};
    this->element->size={size.first-2,size.second-2};
    this->element->redraw = true;
    redraw = false;
  }
  this->element->draw();
}

UIDiv::UIDiv(std::pair<uint16_t,uint16_t> position, std::pair<uint16_t,uint16_t> size, UIDirection dir, std::initializer_list<UIElement*> els, UISize sizing) : UIElement(position,size)
{
  this->dir = dir;
  this->sizing=sizing;
  elements = std::vector<UIElement*>();
  for(UIElement* el : els)
    elements.push_back(el);
}

UIDiv::UIDiv(UIDirection dir, std::initializer_list<UIElement*> els, UISize sizing) : UIElement({0,0},{0,0})
{
  this->dir = dir;
  this->sizing=sizing;
  elements = std::vector<UIElement*>();
  for(UIElement* el : els)
    elements.push_back(el); 
}

void UIDiv::draw()
{
  // Check number of elements in div
  uint16_t count;
  if(!(count = elements.size()))
    return; 
  
  if(redraw)
  {
    if(dir == Vertical) //vertical
    {
      uint16_t prealloted = 0;
      uint16_t sizeablecount = 0;
      for(uint16_t i=0;i<count;i++)
      {
        switch(elements[i]->sizing)
        {
          case Fill:
            sizeablecount++;
            break;
          case Wrap:
            // Get and update element's size
            std::pair<uint16_t,uint16_t> s = (elements[i]->size=elements[i]->getsize());
            // Count the wrapped element as prealloted
            prealloted+=s.second;
            break;
        }
      }
      for(uint16_t y = pos.second,i=0;i<count;i++)
      {
        // TODO add all of the other sizing methods
        elements[i]->pos={pos.first,y}; //update position
        switch(elements[i]->sizing)
        {
          case Fill:
            elements[i]->size={size.first,(size.second-prealloted)/sizeablecount}; //update size
            y+=elements[i]->size.second;
            break;
          case Wrap:
            elements[i]->size.first=size.first; //update size
            y+=elements[i]->size.second;
            break;
        }
        elements[i]->redraw=true;
        elements[i]->draw();
      }
    }
    else //horizontal
    {
      uint16_t prealloted = 0;
      uint16_t sizeablecount = 0;
      for(uint16_t i=0;i<count;i++)
      {
        switch(elements[i]->sizing)
        {
          case Fill:
            sizeablecount++;
            break;
          case Wrap:
            // Get and update element's size
            std::pair<uint16_t,uint16_t> s = (elements[i]->size=elements[i]->getsize());
            // Count the wrapped element as prealloted
            prealloted+=s.first;
            break;
        }
      }
      for(uint16_t x = pos.first,i=0;i<count;i++)
      {
        // TODO add all of the other sizing methods
        elements[i]->pos={x,pos.second}; //update position
        switch(elements[i]->sizing)
        {
          case Fill:
            elements[i]->size={(size.first-prealloted)/sizeablecount,size.second}; //update size
            x+=(size.first-prealloted)/sizeablecount;
            break;
          case Wrap:
            elements[i]->size.second=size.second; //update size
            x+=elements[i]->size.first;
            break;
        }
        elements[i]->redraw=true;
        elements[i]->draw();
      }
    }
    redraw=false;
  }
  else
  {
    for(UIElement* el : elements)
    {
      el->draw();
    }
  }
}

std::pair<uint16_t,uint16_t> UIDiv::getsize()
{
  std::pair<uint16_t,uint16_t> minsize = {0,0};
  if(dir) //Vertical
  {
    for(UIElement* el : elements)
    {
      std::pair<uint16_t,uint16_t> s={0,0};
      switch(el->sizing)
        {
        case Fill: // treat as wrap to get minimum size
        case Wrap:
          s = el->getsize();
        case Set:
          minsize.second+=s.second;
          minsize.first = std::max(s.first,minsize.first); // get width max
          break;
        }
    }
  }
  else
  {
    for(UIElement* el : elements)
    {
      std::pair<uint16_t,uint16_t> s={0,0};
      switch(el->sizing)
        {
        case Fill: // treat as wrap to get minimum size
        case Wrap:
          s = el->getsize();
        case Set:
          minsize.first+=s.first;
          minsize.second = std::max(s.second,minsize.second); // get width max
          break;
        }
    }
  }
  return minsize;
}

UILabel::UILabel(std::string text, int color, UISize sizing) : UIElement(color)
{
  this->text=text;
  this->sizing=sizing;
  this->size={4,4};
}

void UILabel::draw()
{
  //TODO add size checking
  if(redraw)
  {
    attron(COLOR_PAIR(color));
    mvprintw(pos.second,pos.first,text.c_str());
    attroff(COLOR_PAIR(color));
    this->redraw = false;
  }
}

void UILabel::settext(std::string str)
{
  // only redraw label if the text has changed
  if(text != str)
  {
    this->redraw=true;
    this->text = str;
  }
}

std::string UILabel::gettext()
{
  return this->text;
}

std::pair<uint16_t,uint16_t> UILabel::getsize()
{
  //TODO add line wrapping
  return std::make_pair<uint16_t,uint16_t>(text.length(),1);
}
