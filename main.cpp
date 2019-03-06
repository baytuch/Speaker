
#include <iostream>
#include <string>
#include <vector>
#include "configer.h"
#include "sleep.h"
#include "base_link.h"
#include "speak_controller.h"

int main(){
  Configer cfg = Configer();

  Base_link bl = Base_link("mqtt.it-hobby.km.ua", 1883, false);
  Sleep(10000);
  bl.stop();

  /*
  char *name = cfg.getVoiceName();
  std::cout << name << std::endl;
  Speak_controller sc = Speak_controller();
  for (unsigned int n = 0; n < 1000; n++){
    sc.tell("ѕрив≥т");
    sc.tell("як справи");
    //sc.tell("test");
    Sleep(600);
  }
  sc.stop();
  */
}
