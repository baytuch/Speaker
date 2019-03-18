
#ifndef BASE_LINK_H_INCLUDED
#define BASE_LINK_H_INCLUDED

#include <pthread.h>
#include "logger.h"

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
    bool tx(Link_message msg);
    void stop();

  private:
    char *mqtt_uri;
    char *rx_loop_buffer;
    char *tx_loop_buffer;
    char *rx_topic;
    char *rx_body;
    char *tx_topic;
    char *tx_body;
    unsigned int mqtt_port;
    unsigned int rx_push_n;
    unsigned int rx_pull_n;
    unsigned int tx_push_n;
    unsigned int tx_pull_n;
    bool mqtt_ssl;
    bool link_run;
    bool link_is_stop;
    bool init_flag;
    bool client_subscribe_status;
    bool client_rx_status;
    bool client_tx_status;
    bool rx_push_lock;
    bool tx_push_lock;
    bool tx_buffer_is_empty;
    bool rx_buffer_overflow;
    bool tx_buffer_overflow;
    pthread_t loop_tid;
    Logger logger;
    static const char *mqtt_id;
    static const unsigned int rx_topic_length;
    static const unsigned int rx_body_length;
    static const unsigned int rx_size;
    static const unsigned int tx_topic_length;
    static const unsigned int tx_body_length;
    static const unsigned int tx_size;
    void loop();
    static void *init_loop(void *vptr_args);
    void link_init();
    void link_do();
    void link_stop();
    void client_subscribe(const char *topic);
    void client_rx();
    void client_tx();
    void rx_push();
    Link_message rx_pull();
    bool tx_push(Link_message msg);
    void tx_pull();

};

#endif // BASE_LINK_H_INCLUDED
