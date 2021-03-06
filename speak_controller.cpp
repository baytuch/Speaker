
#include <stdlib.h>
#include <string.h>
#include <pthread.h>
#include "utils.h"
#include "sleep.h"
#include "logger.h"
#include "speak_worker.h"
#include "speak_controller.h"


const unsigned int Speak_controller::say_length = 128;
const unsigned int Speak_controller::say_size = 32;

Speak_controller::Speak_controller(){
  this->loop_buffer = strinit(Speak_controller::say_length * Speak_controller::say_size);
  this->worker_run = true;
  this->worker_is_stop = true;
  this->push_n = 0;
  this->pull_n = 0;
  this->push_lock = false;
  this->buffer_overflow = false;
  this->logger = Logger("SPEAK CONTROL");
  pthread_create(&this->loop_tid, NULL, Speak_controller::init_loop, this);
}

Speak_controller::~Speak_controller(){
  //free(this->loop_buffer);
}

void Speak_controller::loop(){
  this->logger << "worker init";
  this->worker_init();
  while(this->worker_run){
    Sleep(250);
    this->worker_do();
  }
  this->worker_stop();
  this->worker_is_stop = false;
  this->logger << "worker stop";
}

void *Speak_controller::init_loop(void *vptr_args){
  ((Speak_controller *)vptr_args)->loop();
  return NULL;
}

void Speak_controller::worker_init(){
  this->worker = Speak_worker();
  this->worker.setVoice("Natalia");
}

void Speak_controller::worker_stop(){
  //this->worker.~Speak_worker();
}

bool Speak_controller::tell(const char *oration){
  bool res = false;
  waiter(this->push_lock);
  size_t oration_length = strlen(oration);
  char c = 0x00;
  if (oration_length < Speak_controller::say_length){
    if ((this->pull_n > 0 && this->push_n + 1 == this->pull_n) ||
        (this->pull_n == 0 && this->push_n == Speak_controller::say_size - 1)){
        this->buffer_overflow = true;
        this->logger << "buffer overflow!";
      } else {
      for (unsigned int n = 0; n < Speak_controller::say_length; n++){
        if (n < oration_length){
          c = oration[n];
        } else {
          c = 0x00;
        }
        this->loop_buffer[this->push_n * Speak_controller::say_length + n] = c;
      }
      if (this->push_n >= Speak_controller::say_size - 1){
        this->push_n = 0;
      } else {
        this->push_n++;
      }
      res = true;
      this->buffer_overflow = false;
    }
  }
  return res;
}

void Speak_controller::worker_do(){
  char *oration;
  char c = 0x00;
  if (this->pull_n != this->push_n){
    oration = strinit(Speak_controller::say_length);
    this->push_lock = true;
    for (unsigned int n = 0; n < Speak_controller::say_length; n++){
      c = this->loop_buffer[this->pull_n * Speak_controller::say_length + n];
      oration[n] = c;
    }
    if (this->pull_n >= Speak_controller::say_size - 1){
      this->pull_n = 0;
    } else {
      this->pull_n++;
    }
    this->push_lock = false;
    this->worker.say(oration);
    free(oration);
  }
}

void Speak_controller::stop(){
  this->worker_run = false;
  waiter(this->worker_is_stop);
}
