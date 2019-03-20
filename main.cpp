
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

  bool cfg_status = false;
  char *voice_name;
  unsigned int voice_volume = 0;
  unsigned int voice_rate = 0;
  char *link_host;
  unsigned int link_port = 0;
  bool link_ssl = false;
  char *oration;
  bool run_main_loop = true;

  Logger logger("CORE");

  Configer cfg = Configer();
  if (cfg.getStatus()){
    voice_name = cfg.getVoiceName();
    voice_volume = cfg.getVoiceVolume();
    voice_rate = cfg.getVoiceRate();
    link_host = cfg.getLinkHost();
    link_port = cfg.getLinkPort();
    link_ssl = cfg.getLinkSSL();
    cfg_status = true;
    logger << "successful loading of settings";
  } else {
    logger << "error loading settings";
  }

  if (cfg_status){
    Base_link bl = Base_link(link_host, link_port, link_ssl, sub_topic);
    Speak_controller sc = Speak_controller();
    Semantic smt = Semantic(smt_db);

    free_mem((void *) voice_name);
    free_mem((void *) link_host);

    logger << "init main loop";

    while(run_main_loop) {
      Link_message msg = bl.rx();
      if (msg.flag){
        oration = smt.compiler(msg.body);
        sc.tell(oration);
        const char *report_heder = "received new command - ";
        char *msg_report = strinit(strlen(report_heder) + strlen(msg.body));
        strcpy(msg_report, report_heder);
        strcat(msg_report, msg.body);
        logger << msg_report;
        free_mem((void *) msg_report);
        free_mem((void *) oration);
      }
      free_mem((void *) msg.topic);
      free_mem((void *) msg.body);
      Sleep(1000);
    }

    logger << "stop main loop";
    //bl.stop();
    //sc.stop();
  }

}
