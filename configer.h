
#ifndef CONFIGER_H_INCLUDED
#define CONFIGER_H_INCLUDED

class Configer {

  public:
    Configer();
    ~Configer();
    bool getStatus();
    char *getVoiceName();
    unsigned int getVoiceVolume();
    int getVoiceRate();

  private:
    bool load_status;
    bool parse_status;
    bool status;
    char *buff;
    char *voice_name;
    unsigned int voice_volume;
    int voice_rate;
    void load();
    void parse();

};

#endif // CONFIGER_H_INCLUDED
