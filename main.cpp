
#include <iostream>
#include <string>
#include <vector>
#include "speak_worker.h"

int main(){
  std::vector<std::string> voices;
  Speak_worker sp = Speak_worker();
  if (sp.getStatus()){
    std::cout << "init ok" << std::endl;
    sp.say();
    sp.getVoices(voices);
    if (sp.getStatus()){
      std::cout << "len ok" << std::endl;
	  for (size_t k = 0; k < voices.size(); k++){
        std::cout << voices[k] << std::endl;
	  }
	}
  }
}
