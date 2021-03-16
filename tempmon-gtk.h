#include <iostream>
#include <string>

#include <client.h>

#include <gtkmm-3.0/gtkmm.h>

class SensorDisplay : public Gtk::Bin
{
public:
  Gtk::Widget child=Gtk::Label("asd1");
  std::string stringvalue="";
  int intvalue=0;
  float floatvalue=0;
  void setString(std::string s)
  {
    stringvalue = s;
    intvalue=-1;
    floatvalue = 0;
    updateValue();
  }
  void setInt(int i)
  {
    stringvalue = std::to_string(i);
    intvalue = i;
    floatvalue = i;
    updateValue();
  }
  void setFloat(float f)
  {
    stringvalue = std::to_string(f);
    intvalue = (int)f;
    floatvalue = f;
    updateValue();
  }
  virtual void updateValue(){std::cout<<"wrong function called"<<std::endl;};
};

class StringDisplay : public SensorDisplay//, public virtual Gtk::Bin
{
public:
  StringDisplay(std::string);
  void updateValue() override;
  //void set_manage(){Gtk::Label::set_manage();}
  //void destroy_notify_(){Gtk::Label::destroy_notify_();}
};

class Sensor : public Gtk::Box
{
public:
  Sensor(std::string, std::unique_ptr<SensorDisplay>);
  Gtk::Label name;
  std::unique_ptr<SensorDisplay> display;
};

class NodeWidget : public Gtk::Box
{
public:
  uint32_t id=0;
  NodeWidget();
  NodeWidget(std::shared_ptr<Client>);
  std::vector<std::unique_ptr<Sensor>> sensors;
  Gtk::Label title;
  
  void update(std::shared_ptr<Client> c);
  
};

class TempmonWindow : public Gtk::Window
{
public:
  TempmonWindow();
  void onbutton();
  void addNode(std::shared_ptr<Client>);
  void deleteNode(std::shared_ptr<Client>);
  void updateNode(std::shared_ptr<Client>);
  // Consider replacing this vector with a hashmap for faster searches
  std::vector<std::shared_ptr<NodeWidget>> nws;
  Gtk::VBox vbox;
};


