
#ifndef CONFIGER_H_INCLUDED
#define CONFIGER_H_INCLUDED

class Configer {

  public:
    Configer();
    ~Configer();
    char *getVoiceName();
    unsigned int getVoiceVolume();
    int getVoiceRate();
    char *getLinkHost();
    unsigned int getLinkPort();
    bool getLinkSSL();
    bool getStatus();

  private:
    bool load_status;
    bool parse_status;
    bool status;
    char *buff;
    char *voice_name;
    unsigned int voice_volume;
    int voice_rate;
    char *link_host;
    unsigned int link_port;
    bool link_ssl;
    void load();
    void parse();

};

#endif // CONFIGER_H_INCLUDED
