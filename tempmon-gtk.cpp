#include <tempmon-gtk.h>
//NodeWidget nw;
TempmonWindow::TempmonWindow()
{
  set_border_width(10);
  add(vbox);
  this->show_all();
}

void TempmonWindow::addNode(std::shared_ptr<Client> c)
{
  // Initialize a node widget and track it inside the vector
  this->nws.push_back(std::make_shared<NodeWidget>(c));
  vbox.pack_end(*this->nws.back());
  // Dispaly the widget
  this->show_all();
}

void TempmonWindow::updateNode(std::shared_ptr<Client> c)
{
  // Search for the corresponding node NodeWidget
  for(std::shared_ptr<NodeWidget> nw : nws)
  {
    // Check for matching unique id
    if(nw->id == c->UID)
    {
      // Trigger widget update
      nw->update(c);
      
      break; // This breaks support for having multiple nodes with the same ID
    }
  }
}

void TempmonWindow::deleteNode(std::shared_ptr<Client> c)
{
  //TODO Implement this stub
  std::cout<<"deleting object"<<std::endl;
}

//TODO remove this constructor
NodeWidget::NodeWidget()
{
  this->sensors.push_back(std::make_unique<Sensor>("henlo:",std::make_unique<StringDisplay>("asdfg")));
  this->pack_start(*this->sensors.back());
}

NodeWidget::NodeWidget(std::shared_ptr<Client> c)
{
  this->id = c->UID;
  for(int i=0; i< c->propertycount; i++)
  {
    this->sensors.push_back(std::make_unique<Sensor>(c->propertynames[i],std::make_unique<StringDisplay>(std::to_string(c->propertyvalues[i]))));
    this->pack_start(*this->sensors.back());
  }
}

void NodeWidget::update(std::shared_ptr<Client> c)
{
  for(int i=0; i< c->propertycount; i++)
  {
    this->sensors[i]->display->setFloat(c->propertyvalues[i]);
  }
}

Sensor::Sensor(std::string name, std::unique_ptr<SensorDisplay> display)
{
  this->name = Gtk::Label(name);
  this->display = std::move(display);
  this->pack_start(this->name);
  this->pack_start(*this->display);
}

StringDisplay::StringDisplay(std::string s)
{
  this->child = Gtk::Label(s);
  this->add(this->child);
}

void StringDisplay::updateValue()
{
  ((Gtk::Label*)(this->get_child()))->set_text(this->stringvalue);
}
