
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

  Link_message tx_msg;
  tx_msg.topic = "test2";
  tx_msg.body = "hello";
  tx_msg.flag = 0;

  while(true) {
    Link_message msg = bl.rx();
    bl.tx(tx_msg);
    if (msg.flag){
      oration = smt.compiler(msg.body);
      std::cout << oration << std::endl;
      sc.tell(oration);
    }
    //free(oration);
    free(msg.topic);
    free(msg.body);
    Sleep(1000);
  }

}
