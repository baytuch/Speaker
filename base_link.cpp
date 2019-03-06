
#include <iostream>
#include <string.h>
#include <pthread.h>
#include <MQTTClient.h>
#include "sleep.h"
#include "utils.h"
#include "base_link.h"


Base_link::Base_link(const char *host, const unsigned int &port, const bool &ssl){
  this->mqtt_host = strcopy(host);
  this->mqtt_port = port;
  this->mqtt_ssl = ssl;
  this->link_run = true;
  this->link_is_stop = true;
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
}

void Base_link::link_do(){
}

void Base_link::link_stop(){
}

void Base_link::stop(){
  this->link_run = false;
  waiter(this->link_is_stop);
}
