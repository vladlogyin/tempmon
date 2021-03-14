#include <iostream>
#include <thread>
#include <chrono>
#include <algorithm>
#include <vector>
#include <string>
#include <utility>
#include <initializer_list>

#include <curses.h>
#include <menu.h>

// Root uses whole screen
// Fill gets sized by parent. Same as wrap if parent is also Wrap sized
// Wrap is sized by content
// Set forces the set size

enum UISize : int {Root=0, Fill=1, Wrap=2, Set=3};

enum UIDirection : int {Horizontal=0, Vertical=1};

class UIElement {
public:
  UIElement(std::pair<uint16_t,uint16_t>, std::pair<uint16_t,uint16_t>);
  UIElement(int);
  
  std::pair<uint16_t,uint16_t> pos, size;

  uint32_t id;
  
  int color;
  UISize sizing;
  // Flag used to determine whether to redraw whole element or just to update it
  bool redraw = true;
  virtual void draw(){};
  // Dummy getsize function used for debugging
  virtual std::pair<uint16_t, uint16_t> getsize(){return {4,4};};
};

//#define UIBox UIElement


class UIBox : public UIElement {
public:
  UIBox(int);
  
  void draw() override;
};

class UIFrame : public UIElement {
public:
  UIFrame(UIElement*,int);
  UIElement* element;
  
  void draw() override;
};

class UIDiv : public UIElement {
public:
  UIDiv(std::pair<uint16_t,uint16_t>, std::pair<uint16_t,uint16_t>, UIDirection, std::initializer_list<UIElement*>, UISize = Set);
  UIDiv(UIDirection, std::initializer_list<UIElement*>, UISize = Fill);
  std::vector<UIElement*> elements;
  UIDirection dir;
  
  void draw() override;
  std::pair<uint16_t, uint16_t> getsize() override;
};

class UIBar : public UIElement {
public:
  UIBar();
  
  uint8_t percentage;
  
  void draw() override;
  //std::pair<uint16_t, uint16_t> getsize() override{};
private:
  uint8_t last_percentage;
};

class UILabel : public UIElement {
public:
  //TODO have label printed in center of alloted space when Fill or Set sizing is used
  UILabel(std::string, int, UISize = Wrap);
  
  void settext(std::string);
  std::string gettext(void);
  
  void draw() override;
  std::pair<uint16_t, uint16_t> getsize() override;
  
private:
    std::string text;
};
