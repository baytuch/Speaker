
#include <iostream>
#include <string>
#include <vector>
#include "speak_worker.h"

int main(){
  std::vector<std::string> voices;
  std::string voice;
  unsigned int vol = 0;
  int rate = 0;
  Speak_worker sp = Speak_worker();
  if (sp.getStatus()){
    vol = sp.getVolume();
	rate = sp.getRate();
    std::cout << "init ok" << std::endl;
    std::cout << "Volume: " << vol << std::endl;
    std::cout << "Rate: " << rate << std::endl;
    sp.say(L"test");
    sp.getVoices(voices);
    if (sp.getStatus()){
      std::cout << "len ok" << std::endl;
	  for (size_t k = 0; k < voices.size(); k++){
        std::cout << voices[k] << std::endl;
	  }
	}
    sp.getVoice(voice);
    if (sp.getStatus()){
      std::cout << "voice ok" << std::endl;
      std::cout << voice << std::endl;
	}
	sp.setVoice("Natalia");
    sp.getVoice(voice);
    if (sp.getStatus()){
      std::cout << "voice ok" << std::endl;
      std::cout << voice << std::endl;
    }
    std::cout << "Update parms" << std::endl;
	sp.setRate(-1);
	sp.setVolume(30);
    vol = sp.getVolume();
    rate = sp.getRate();
    std::cout << "Volume: " << vol << std::endl;
    std::cout << "Rate: " << rate << std::endl;
    if (sp.getStatus()){
      sp.say(L"Привіт, як справи");
	}
  }
}
