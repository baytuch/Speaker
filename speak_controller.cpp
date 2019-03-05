
#include <iostream>
#include <stdlib.h>
#include <string.h>
#include <pthread.h>
#include "utils.h"
#include "sleep.h"
#include "speak_worker.h"
#include "speak_controller.h"


const unsigned int Speak_controller::say_length = 128;
const unsigned int Speak_controller::say_size = 32;

Speak_controller::Speak_controller(){
  this->loop_buffer = strinit(Speak_controller::say_length * Speak_controller::say_size);
  pthread_create(&this->loop_tid, NULL, Speak_controller::init_loop, this);
}

Speak_controller::~Speak_controller(){
  free(this->loop_buffer);
}

void Speak_controller::loop(){
  std::cout << "loop init" << std::endl;
  while(true){
    Sleep(1000);
    std::cout << "loop do" << std::endl;
  }
}

void *Speak_controller::init_loop(void *vptr_args){
  ((Speak_controller *)vptr_args)->loop();
  return NULL;
}
