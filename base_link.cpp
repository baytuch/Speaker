
#include <iostream>
#include <string.h>
#include <pthread.h>
#include <MQTTClient.h>
#include "sleep.h"
#include "utils.h"
#include "base_link.h"


const char *Base_link::mqtt_id = "IrisVoice";
const unsigned int Base_link::rx_topic_length = 32;
const unsigned int Base_link::rx_body_length = 128;
const unsigned int Base_link::rx_size = 64;

Base_link::Base_link(const char *host, const unsigned int &port, const bool &ssl){
  this->mqtt_host = strcopy(host);
  this->mqtt_port = port;
  this->mqtt_ssl = ssl;
  this->link_run = true;
  this->link_is_stop = true;
  this->client_subscribe_status = false;
  this->client_rx_status = false;
  this->rx_topic = strinit(Base_link::rx_topic_length);
  this->rx_body = strinit(Base_link::rx_body_length);
  this->rx_loop_buffer = strinit((Base_link::rx_topic_length + Base_link::rx_body_length) * Base_link::rx_size);
  this->rx_push_n = 0;
  this->rx_pull_n = 0;
  pthread_create(&this->loop_tid, NULL, Base_link::init_loop, this);
}

Base_link::~Base_link(){
}

void Base_link::loop(){
  std::cout << "link init" << std::endl;
  this->link_init();
  while(this->link_run){
    Sleep(1000);
    std::cout << "link do" << std::endl;
    this->link_do();
  }
  this->link_stop();
  this->link_is_stop = false;
  std::cout << "link stop" << std::endl;
}

void *Base_link::init_loop(void *vptr_args){
  ((Base_link *)vptr_args)->loop();
  return NULL;
}

void Base_link::link_init(){
  this->client_subscribe("/test");
}

void Base_link::link_do(){
  this->client_rx();
  if (this->client_rx_status){
    std::cout << this->rx_topic << std::endl;
    std::cout << this->rx_body << std::endl;
    this->rx_push();
  }
}

void Base_link::link_stop(){
}

void Base_link::client_subscribe(const char *topic){
  MQTTClient client;
  MQTTClient_connectOptions conn_opts = MQTTClient_connectOptions_initializer;
  this->client_subscribe_status = false;
  MQTTClient_create(&client, this->mqtt_host, Base_link::mqtt_id, MQTTCLIENT_PERSISTENCE_NONE, NULL);
  conn_opts.keepAliveInterval = 20;
  conn_opts.cleansession = 0;
  if (MQTTClient_connect(client, &conn_opts) == MQTTCLIENT_SUCCESS){
    if (MQTTClient_subscribe(client, topic, 2) == MQTTCLIENT_SUCCESS){
      this->client_subscribe_status = true;
      std::cout << "subscribed" << std::endl;
    }
    MQTTClient_disconnect(client, 10000);
    MQTTClient_destroy(&client);
  } else {
    this->client_rx_status = false;
    std::cout << "Link: error connect!" << std::endl;
  }
}

void Base_link::client_rx(){
  MQTTClient client;
  MQTTClient_connectOptions conn_opts = MQTTClient_connectOptions_initializer;
  MQTTClient_message *pubmsg;
  char *topic_name = NULL;
  int topic_len = 0;
  this->client_rx_status = false;
  MQTTClient_create(&client, this->mqtt_host, Base_link::mqtt_id, MQTTCLIENT_PERSISTENCE_NONE, NULL);
  conn_opts.keepAliveInterval = 20;
  conn_opts.cleansession = 0;
  memset(this->rx_topic, 0x00, Base_link::rx_topic_length);
  memset(this->rx_body, 0x00, Base_link::rx_body_length);
  if (MQTTClient_connect(client, &conn_opts) == MQTTCLIENT_SUCCESS){
    if (MQTTClient_receive(client, &topic_name, &topic_len, &pubmsg, 500L) == MQTTCLIENT_SUCCESS){
      if (topic_name){
        strncpy(this->rx_topic, topic_name, Base_link::rx_topic_length);
        strncpy(this->rx_body, (char *) pubmsg->payload, Base_link::rx_body_length);
        MQTTClient_free(topic_name);
        MQTTClient_freeMessage(&pubmsg);
        this->client_rx_status = true;
      }
    }
    MQTTClient_disconnect(client, 10000);
    MQTTClient_destroy(&client);
  } else {
    this->client_rx_status = false;
    std::cout << "Link: error connect!" << std::endl;
  }
}

void Base_link::rx_push(){
  size_t topic_length = strlen(this->rx_topic);
  size_t body_length = strlen(this->rx_body);
  char c = 0x00;
  for (unsigned int n = 0; n < Base_link::rx_topic_length + Base_link::rx_body_length; n++){
    if (n < Base_link::rx_topic_length){
      if (n < topic_length){
        c = this->rx_topic[n];
      } else {
        c = 0x00;
      }
    } else {
      if (n - Base_link::rx_topic_length < body_length){
        c = this->rx_body[n - Base_link::rx_topic_length];
      } else {
        c = 0x00;
      }
    }
    this->rx_loop_buffer[this->rx_push_n * (Base_link::rx_topic_length + Base_link::rx_body_length) + n] = c;
  }
  if (this->rx_push_n >= Base_link::rx_size - 1){
    this->rx_push_n = 0;
  } else {
    this->rx_push_n++;
  }
  std::cout << "rx_push_n: " << rx_push_n << std::endl; 
}

Link_message Base_link::rx(){
  Link_message msg;
  msg.topic = strinit(Base_link::rx_topic_length);
  msg.body = strinit(Base_link::rx_body_length);
  msg.flag = 0;
  char c = 0x00;
  if (this->rx_pull_n != this->rx_push_n){
    for (unsigned int n = 0; n < Base_link::rx_topic_length + Base_link::rx_body_length; n++){
      c = this->rx_loop_buffer[this->rx_pull_n * (Base_link::rx_topic_length + Base_link::rx_body_length) + n];
      if (n < Base_link::rx_topic_length){
        msg.topic[n] = c;
      } else {
        msg.body[n - Base_link::rx_topic_length] = c;
      }
    }
    if (this->rx_pull_n >= Base_link::rx_size - 1){
      this->rx_pull_n = 0;
    } else {
      this->rx_pull_n++;
    }
    msg.flag = 1;
    std::cout << "rx_pull_n: " << rx_pull_n << std::endl; 
  }
  return msg;
}

void Base_link::stop(){
  this->link_run = false;
  waiter(this->link_is_stop);
}
