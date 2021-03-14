#include <iostream>
#include <string>

#include <gtkmm-3.0/gtkmm.h>

class TempmonWindow : public Gtk::Window
{
public:
  TempmonWindow();
  void onbutton();
//  void addNode(std::shared_ptr<Client> );
//  void deleteNode(std::shared_ptr<Client>);
  Gtk::Button button;
  Gtk::Box hbox;
};

class NodeWidget : public Gtk::Box
{
public:
  uint32_t id=0;
  NodeWidget();
};

class NameValuePair : public Gtk::Box
{
public:
  NameValuePair(std::string, std::string);
  std::string name,value;
};
