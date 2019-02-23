
#ifndef SPEAK_WORKER_H_INCLUDED
#define SPEAK_WORKER_H_INCLUDED

#include <string>
#include <vector>
#include <atlbase.h>
#include <sapi.h>
#include <sphelper.h>


class Speak_worker {

  public:
    Speak_worker();
    ~Speak_worker();
    void say();
    void getVoices(std::vector<std::string> &voices);
    bool getStatus();

  private:
    CComPtr<ISpVoice> voice;
    CComPtr<IEnumSpObjectTokens> voices_info;
    bool status;

};

#endif // SPEAK_WORKER_H_INCLUDED
