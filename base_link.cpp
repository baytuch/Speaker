
#include <string.h>
#include <pthread.h>
#include <MQTTClient.h>
#include "sleep.h"
#include "utils.h"
#include "logger.h"
#include "base_link.h"


const char *Base_link::mqtt_id = "IrisVoice";
const unsigned int Base_link::rx_topic_length = 32;
const unsigned int Base_link::rx_body_length = 128;
const unsigned int Base_link::rx_size = 64;
const unsigned int Base_link::tx_topic_length = 32;
const unsigned int Base_link::tx_body_length = 128;
const unsigned int Base_link::tx_size = 64;

Base_link::Base_link(const char *host, const unsigned int &port, const bool &ssl){
  this->mqtt_host = strcopy(host);
  this->rx_loop_buffer = strinit((Base_link::rx_topic_length + Base_link::rx_body_length) * Base_link::rx_size);
  this->tx_loop_buffer = strinit((Base_link::tx_topic_length + Base_link::tx_body_length) * Base_link::tx_size);
  this->rx_topic = strinit(Base_link::rx_topic_length);
  this->rx_body = strinit(Base_link::rx_body_length);
  this->tx_topic = strinit(Base_link::tx_topic_length);
  this->tx_body = strinit(Base_link::tx_body_length);
  this->mqtt_port = port;
  this->rx_push_n = 0;
  this->rx_pull_n = 0;
  this->tx_push_n = 0;
  this->tx_pull_n = 0;
  this->mqtt_ssl = ssl;
  this->link_run = true;
  this->link_is_stop = true;
  this->init_flag = false;
  this->client_subscribe_status = false;
  this->client_rx_status = false;
  this->client_tx_status = true;
  this->rx_push_lock = false;
  this->tx_push_lock = false;
  this->tx_buffer_is_empty = false;
  this->rx_buffer_overflow = false;
  this->tx_buffer_overflow = false;
  this->logger = Logger("BASE LINK");
  pthread_create(&this->loop_tid, NULL, Base_link::init_loop, this);
}

Base_link::~Base_link(){
}

void Base_link::loop(){
  this->logger << "init";
  this->link_init();
  while(this->link_run){
    this->link_do();
    Sleep(1000);
  }
  this->link_stop();
  this->link_is_stop = false;
  this->logger << "stop";
}

void *Base_link::init_loop(void *vptr_args){
  ((Base_link *)vptr_args)->loop();
  return NULL;
}

void Base_link::link_init(){
  this->init_flag = true;
}

void Base_link::link_do(){
  if (this->init_flag){
    this->client_subscribe("/test");
  } else {
    if (!this->rx_buffer_overflow) this->client_rx();
    if (this->client_rx_status) this->rx_push();
    if (this->client_tx_status) this->tx_pull();
    if (!this->tx_buffer_is_empty) this->client_tx();
  }
  if (this->client_subscribe_status && this->init_flag){
    this->init_flag = false;
    this->logger << "started";
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
      this->logger << "subscribed";
    }
    MQTTClient_disconnect(client, 10000);
    MQTTClient_destroy(&client);
  } else {
    this->client_rx_status = false;
    this->logger << "error connect!";
  }
}

void Base_link::client_rx(){
  MQTTClient client;
  MQTTClient_connectOptions conn_opts = MQTTClient_connectOptions_initializer;
  MQTTClient_message *pubmsg;
  char *topic_name = NULL;
  int topic_len = 0;
  int body_len = 0;
  this->client_rx_status = false;
  MQTTClient_create(&client, this->mqtt_host, Base_link::mqtt_id, MQTTCLIENT_PERSISTENCE_NONE, NULL);
  conn_opts.keepAliveInterval = 20;
  conn_opts.cleansession = 0;
  memset(this->rx_topic, 0x00, Base_link::rx_topic_length);
  memset(this->rx_body, 0x00, Base_link::rx_body_length);
  if (MQTTClient_connect(client, &conn_opts) == MQTTCLIENT_SUCCESS){
    if (MQTTClient_receive(client, &topic_name, &topic_len, &pubmsg, 500L) == MQTTCLIENT_SUCCESS){
      if (topic_name){
        body_len = pubmsg->payloadlen;
        if (topic_len < Base_link::rx_topic_length - 1 && body_len < Base_link::rx_body_length - 1){
          strncpy(this->rx_topic, topic_name, (size_t) topic_len);
          strncpy(this->rx_body, (char *) pubmsg->payload, (size_t) body_len);
          this->client_rx_status = true;
        }
        MQTTClient_free(topic_name);
        MQTTClient_freeMessage(&pubmsg);
      }
    }
    MQTTClient_disconnect(client, 10000);
    MQTTClient_destroy(&client);
  } else {
    this->client_rx_status = false;
    this->logger << "error connect!";
  }
}

void Base_link::client_tx(){
  MQTTClient client;
  MQTTClient_connectOptions conn_opts = MQTTClient_connectOptions_initializer;
  MQTTClient_message pubmsg = MQTTClient_message_initializer;
  MQTTClient_deliveryToken token;
  this->client_tx_status = false;
  MQTTClient_create(&client, this->mqtt_host, Base_link::mqtt_id, MQTTCLIENT_PERSISTENCE_NONE, NULL);
  conn_opts.keepAliveInterval = 20;
  conn_opts.cleansession = 0;
  if (MQTTClient_connect(client, &conn_opts) == MQTTCLIENT_SUCCESS){
    pubmsg.payload = (char*) this->tx_body;
    pubmsg.payloadlen = strlen(this->tx_body);
    pubmsg.qos = 2;
    pubmsg.retained = 0;
    MQTTClient_publishMessage(client, this->tx_topic, &pubmsg, &token);
    if (MQTTClient_waitForCompletion(client, token, 10000L) == MQTTCLIENT_SUCCESS){
      this->client_tx_status = true;
    }
    MQTTClient_disconnect(client, 10000);
    MQTTClient_destroy(&client);
  } else {
    this->client_tx_status = false;
    this->logger << "error connect!";
  }
}

void Base_link::rx_push(){
  size_t topic_length = strlen(this->rx_topic);
  size_t body_length = strlen(this->rx_body);
  char c = 0x00;
  if ((this->rx_pull_n > 0 && this->rx_push_n + 1 == this->rx_pull_n) ||
    (this->rx_pull_n == 0 && this->rx_push_n == Base_link::rx_size - 1)){
    this->rx_buffer_overflow = true;
    this->logger << "rx buffer overflow!";
  } else {
    waiter(this->rx_push_lock);
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
    this->rx_buffer_overflow = false;
  }
}

Link_message Base_link::rx_pull(){
  Link_message msg;
  msg.topic = strinit(Base_link::rx_topic_length);
  msg.body = strinit(Base_link::rx_body_length);
  msg.flag = 0;
  char c = 0x00;
  this->rx_push_lock = true;
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
  }
  this->rx_push_lock = false;
  return msg;
}

bool Base_link::tx_push(Link_message msg){
  size_t topic_length = strlen(msg.topic);
  size_t body_length = strlen(msg.body);
  char c = 0x00;
  bool res = false;
  if (topic_length < Base_link::tx_topic_length - 1 && body_length < Base_link::tx_body_length - 1){
    if ((this->tx_pull_n > 0 && this->tx_push_n + 1 == this->tx_pull_n) ||
        (this->tx_pull_n == 0 && this->tx_push_n == Base_link::tx_size - 1)){
        this->tx_buffer_overflow = true;
        this->logger << "tx buffer overflow!";
      } else {
      waiter(this->tx_push_lock);
      for (unsigned int n = 0; n < Base_link::tx_topic_length + Base_link::tx_body_length; n++){
        if (n < Base_link::tx_topic_length){
          if (n < topic_length){
            c = msg.topic[n];
          } else {
            c = 0x00;
          }
        } else {
          if (n - Base_link::tx_topic_length < body_length){
            c = msg.body[n - Base_link::rx_topic_length];
          } else {
            c = 0x00;
          }
        }
        this->tx_loop_buffer[this->tx_push_n * (Base_link::tx_topic_length + Base_link::tx_body_length) + n] = c;
      }
      if (this->tx_push_n >= Base_link::tx_size - 1){
        this->tx_push_n = 0;
      } else {
        this->tx_push_n++;
      }
      this->tx_buffer_overflow = false;
      res = true;
    }
  }
  return res;
}

void Base_link::tx_pull(){
  char c = 0x00;
  if (this->tx_pull_n != this->tx_push_n){
    memset(this->tx_topic, 0x00, Base_link::tx_topic_length);
    memset(this->tx_body, 0x00, Base_link::tx_body_length);
    this->tx_push_lock = true;
    for (unsigned int n = 0; n < Base_link::tx_topic_length + Base_link::tx_body_length; n++){
      c = this->tx_loop_buffer[this->tx_pull_n * (Base_link::tx_topic_length + Base_link::tx_body_length) + n];
      if (n < Base_link::tx_topic_length){
        this->tx_topic[n] = c;
      } else {
        this->tx_body[n - Base_link::tx_topic_length] = c;
      }
    }
    if (this->tx_pull_n >= Base_link::tx_size - 1){
      this->tx_pull_n = 0;
    } else {
      this->tx_pull_n++;
    }
    this->tx_push_lock = false;
    this->tx_buffer_is_empty = false;
  } else {
    this->tx_buffer_is_empty = true;
  }
}

bool Base_link::tx(Link_message msg){
  return this->tx_push(msg);
}

Link_message Base_link::rx(){
  return this->rx_pull();
}

void Base_link::stop(){
  this->link_run = false;
  waiter(this->link_is_stop);
}
