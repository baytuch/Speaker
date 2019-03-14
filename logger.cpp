
#include <stdio.h>
#include <string.h>
#include <time.h>
#include "utils.h"
#include "logger.h"


const unsigned int Logger::msg_length = 256;

bool Logger::update_lock = false;

Logger::Logger(const char *srv_name, const char *log_file){
  this->srv_name = strcopy(srv_name);
  this->log_file = strcopy(log_file);
  this->msg = strinit(Logger::msg_length);
  Logger::update_lock = false;
}

Logger::~Logger(){
}

void Logger::add_date(){
  const unsigned int buff_size = 80;
  time_t now = time(0);
  struct tm tstruct;
  char buff[buff_size];
  tstruct = *localtime(&now);
  memset(buff, 0x00, buff_size);
  strftime(buff, sizeof(buff), "[%Y-%m-%d %H:%M:%S]", &tstruct);
  strcat(this->msg, buff);
}

void Logger::add_name(){
  const unsigned int name_max_len = 32;
  const char *name_prefix = " ";
  const char *name_sufix = ": ";
  strcat(this->msg, name_prefix);
  if (strlen(this->srv_name) <= name_max_len){
    strcat(this->msg, this->srv_name);
    strcat(this->msg, name_sufix);
  }
}

void Logger::write_msg(){
  FILE *fd;
  char c = 0x00;
  const char *end_line = "\r";
  fd = fopen (this->log_file,"a+");
  if (fd != NULL){
    for (unsigned int n = 0; n < Logger::msg_length; n++){
      c = this->msg[n];
      if (c != 0x00){
        fputc((int) c, fd);
      } else {
        fputs(end_line, fd);
        break;
      }
    }
    fclose (fd);
  }
}

void Logger::show_msg(){
  printf("%s \n", this->msg);
}

void Logger::do_logger(const char *msg){
  const unsigned int msg_max_len = 128;
  waiter(Logger::update_lock);
  Logger::update_lock = true;
  memset(this->msg, 0x00, Logger::msg_length);
  this->add_date();
  this->add_name();
  if (strlen(msg) <= 128){
    strcat(this->msg, msg);
  }
  this->write_msg();
  this->show_msg();
  Logger::update_lock = false;
}

void Logger::operator<< (const char *msg){
  this->do_logger(msg);
}
