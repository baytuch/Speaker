
#ifndef BASE_LINK_H_INCLUDED
#define BASE_LINK_H_INCLUDED

#include <pthread.h>

struct Link_message {
  char *topic;
  char *body;
  int flag;
};

class Base_link {

  public:
    Base_link(const char *host, const unsigned int &port, const bool &ssl);
    ~Base_link();
    Link_message rx();
    void stop();

  private:
    char *rx_loop_buffer;
    char *mqtt_host;
    char *rx_topic;
    char *rx_body;
    unsigned int mqtt_port;
    unsigned int rx_push_n;
    unsigned int rx_pull_n;
    bool mqtt_ssl;
    bool link_run;
    bool link_is_stop;
    bool client_subscribe_status;
    bool client_rx_status;
    pthread_t loop_tid;
    static const char *mqtt_id;
    static const unsigned int rx_topic_length;
    static const unsigned int rx_body_length;
    static const unsigned int rx_size;
    void loop();
    static void *init_loop(void *vptr_args);
    void link_init();
    void link_do();
    void link_stop();
    void client_subscribe(const char *topic);
    void client_rx();
    void rx_push();

};

#endif // BASE_LINK_H_INCLUDED
