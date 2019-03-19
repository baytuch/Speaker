
#include <iostream>
#include "sleep.h"
#include "configer.h"
#include "base_link.h"
#include "speak_controller.h"
#include "semantic.h"
#include "utils.h"
#include "logger.h"


const char *sub_topic = "/iris/voice/";
const char *smt_db = "smt.json";

int main(){

  char *voice_name;
  unsigned int voice_volume = 0;
  unsigned int voice_rate = 0;
  char *link_host;
  unsigned int link_port = 0;
  bool link_ssl = false;

  Configer cfg = Configer();
  if (cfg.getStatus()){
    voice_name = cfg.getVoiceName();
    voice_volume = cfg.getVoiceVolume();
    voice_rate = cfg.getVoiceRate();
    link_host = cfg.getLinkHost();
    link_port = cfg.getLinkPort();
    link_ssl = cfg.getLinkSSL();
  }

  Base_link bl = Base_link(link_host, link_port, link_ssl, sub_topic);
  Speak_controller sc = Speak_controller();
  Semantic smt = Semantic(smt_db);
  Logger logger("CORE");
  char *oration;

  free_mem(voice_name);
  free_mem(link_host);

  logger << "init...";

  Link_message tx_msg;
  tx_msg.topic = "test2";
  tx_msg.body = "hello";
  tx_msg.flag = 0;

  while(true) {
    Link_message msg = bl.rx();
    //bl.tx(tx_msg);
    if (msg.flag){
      oration = smt.compiler(msg.body);
      logger << oration;
      sc.tell(oration);
    }
    //free(oration);
    free(msg.topic);
    free(msg.body);
    Sleep(1000);
  }

}
