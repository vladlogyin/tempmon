#include <tempmon-gtk.h>

TempmonWindow::TempmonWindow()
{
  set_border_width(10);
  button.signal_clicked().connect(sigc::mem_fun(*this,&TempmonWindow::onbutton));
  add(button);
  button.show();
}

void TempmonWindow::onbutton()
{
  std::cout<<"henlo"<<std::endl;
}
