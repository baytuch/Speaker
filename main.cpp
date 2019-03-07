
#include <iostream>
#include "sleep.h"
#include "configer.h"
#include "base_link.h"
#include "speak_controller.h"
#include "semantic.h"


int main(){

  Configer cfg = Configer();
  Base_link bl = Base_link("mqtt.it-hobby.km.ua", 1883, false);
  Speak_controller sc = Speak_controller();
  Semantic smt = Semantic("smt.json");
  char *oration;

  while(true) {
    Link_message msg = bl.rx();
    if (msg.flag){
      oration = smt.compiler(msg.body);
      std::cout << oration << std::endl;
      sc.tell(oration);
    }
    //free(oration);
    free(msg.topic);
    free(msg.body);
    Sleep(250);
  }

}
