
#ifndef BASE_LINK_H_INCLUDED
#define BASE_LINK_H_INCLUDED

#include <pthread.h>

class Base_link {

  public:
    Base_link(const char *host, const unsigned int &port, const bool &ssl);
    ~Base_link();
    void stop();

  private:
    char *mqtt_host;
    unsigned int mqtt_port;
    bool mqtt_ssl;
    bool link_run;
    bool link_is_stop;
    bool client_subscribe_status;
    bool client_rx_status;
    pthread_t loop_tid;
    static const char *mqtt_id;
    void loop();
    static void *init_loop(void *vptr_args);
    void link_init();
    void link_do();
    void link_stop();
    void client_subscribe(const char *topic);
    void client_rx();

};

#endif // BASE_LINK_H_INCLUDED
