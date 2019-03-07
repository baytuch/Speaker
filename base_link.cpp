
#include <iostream>
#include <string.h>
#include <pthread.h>
#include <MQTTClient.h>
#include "sleep.h"
#include "utils.h"
#include "base_link.h"


const char *Base_link::mqtt_id = "IrisVoice";

Base_link::Base_link(const char *host, const unsigned int &port, const bool &ssl){
  this->mqtt_host = strcopy(host);
  this->mqtt_port = port;
  this->mqtt_ssl = ssl;
  this->link_run = true;
  this->link_is_stop = true;
  this->client_subscribe_status = false;
  this->client_rx_status = false;
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
  //this->doEraseRxMsg();
  if (MQTTClient_connect(client, &conn_opts) == MQTTCLIENT_SUCCESS){
    if (MQTTClient_receive(client, &topic_name, &topic_len, &pubmsg, 500L) == MQTTCLIENT_SUCCESS){
      if (topic_name){
        //strncpy(this->rx_msg.topic, topic_name, BASELEN);
        //strncpy(this->rx_msg.body, (char *) pubmsg->payload, BUFFLEN);
        std::cout << (char *) pubmsg->payload << std::endl;
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

void Base_link::stop(){
  this->link_run = false;
  waiter(this->link_is_stop);
}
