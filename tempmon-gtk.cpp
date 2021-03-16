#include <tempmon-gtk.h>
//NodeWidget nw;
TempmonWindow::TempmonWindow()
{
  set_border_width(10);
  //button.signal_clicked().connect(sigc::mem_fun(*this,&TempmonWindow::onbutton));
  //nodetest = NodeWidget();
  //Gtk::Box b;
  //add(hbox);
  //nw
  //hbox.pack_end(nw);
  this->show_all();
}

void TempmonWindow::onbutton()
{
  std::cout<<"henlo"<<std::endl;
}

NodeWidget::NodeWidget() : Gtk::Box()
{
  //this->sen = Sensor("henlo:",SensorDisplay());
  //this->sen.display.setInt();
  //this->pack_start(sen);
}

NodeWidget::NodeWidget(std::shared_ptr<Client> c)
{
  this->id = c->UID;
  //this->
}

Sensor::Sensor(std::string name, SensorDisplay display) : Gtk::Box()
{
  this->name = Gtk::Label(name);
  this->display = std::move(display);//StringDisplay("worl");//std::move(display);
  this->pack_start(this->name);
  //this->pack_start(this->display);
}

StringDisplay::StringDisplay(std::string s) : SensorDisplay()
{
  //this->value = Gtk::Label("asd");
  //this->set_label("asdf");
  //this->set_text(s);
  //show_all();
}
