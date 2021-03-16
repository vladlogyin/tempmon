#include <iostream>
#include <string>

#include <client.h>

#include <gtkmm-3.0/gtkmm.h>

class SensorDisplay : public Gtk::Container
{
public:
  Gtk::Label label = Gtk::Label("asdfg");
  SensorDisplay() : Gtk::Container()
  {
    this->add(label);
  };
  std::string stringvalue="";
  int intvalue=0;
  float floatvalue=0;
  void setString(std::string s)
  {
    stringvalue = s;
    intvalue=-1;
    floatvalue = 0;
  }
  void setInt(int i)
  {
    stringvalue = std::to_string(i);
    intvalue = i;
    floatvalue = i;
  }
  void setFloat(float f)
  {
    stringvalue = std::to_string(f);
    intvalue = (int)f;
    floatvalue = f;
  }
};

class StringDisplay : public SensorDisplay
{
public:
  Gtk::Label value;
  StringDisplay(){};
  StringDisplay(std::string);
  //void set_manage(){Gtk::Label::set_manage();}
  //void destroy_notify_(){Gtk::Label::destroy_notify_();}
};

class Sensor : public Gtk::Box
{
public:
  Sensor(){};
  Sensor(std::string, SensorDisplay);
  Gtk::Label name;
  SensorDisplay display;
};

class NodeWidget : public Gtk::Box
{
public:
  uint32_t id=0;
  NodeWidget();
  NodeWidget(std::shared_ptr<Client>);
  //Sensor sen;
  Gtk::Label title;
  
};

class TempmonWindow : public Gtk::Window
{
public:
  TempmonWindow();
  void onbutton();
  void addNode(std::shared_ptr<Client>);
  void deleteNode(std::shared_ptr<Client>);
  void updateNode(std::shared_ptr<Client>);
  //Gtk::Button button;
  //NodeWidget nw;
  //Gtk::Box hbox;
};


