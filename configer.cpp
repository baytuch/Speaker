
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <jansson.h>
#include "utils.h"
#include "configer.h"
#ifndef SP_CONF
#define SP_CONF "config.json"
#endif

Configer::Configer(){
  this->load_status = false;
  this->parse_status = false;
  this->status = false;
  this->buff = "";
  this->voice_name = "";
  this->link_host = "";
  this->link_port = 0;
  this->link_ssl = false;
  this->load();
  if (this->load_status){
    this->parse();
    if (this->parse_status) this->status = true;
  }
}

Configer::~Configer(){
  //free(this->voice_name);
}

void Configer::load(){
  const unsigned int buff_size = 8192;
  FILE *fd;
  int c;
  this->load_status = false;
  fd = fopen(SP_CONF, "r");
  if (fd != NULL){
    this->buff = strinit(buff_size);
    for (unsigned int n = 0; n < buff_size; n++){
      c = fgetc(fd);
      if (c != EOF){
        this->buff[n] = (char) c;
      } else {
        this->buff[n] = 0x00;
        if (n > 0) this->load_status = true;
        break;
      }
    }
    fclose(fd);
  }
}

void Configer::parse(){
  json_t *json;
  json_error_t error;
  json_t *voice_obj;
  json_t *link_obj;
  json_t *name_obj;
  json_t *volume_obj;
  json_t *rate_obj;
  json_t *host_obj;
  json_t *port_obj;
  json_t *ssl_obj;
  int num_buff = 0;
  unsigned int status_n = 0;
  this->parse_status = false;
  json = json_loads(this->buff, 0, &error);
  free(this->buff);
  if(json){
    if (json_is_object(json)){
      voice_obj = json_object_get(json, "voice");
      if (json_is_object(voice_obj)){
        name_obj = json_object_get(voice_obj, "name");
        if (json_is_string(name_obj)){
          this->voice_name = strcopy(json_string_value(name_obj));
          if (strlen(this->voice_name) > 0) status_n++;
          json_object_clear(name_obj);
        }
        volume_obj = json_object_get(voice_obj, "volume");
        if (json_is_integer(volume_obj)){
          num_buff = 0;
          num_buff = (int) json_integer_value(volume_obj);
          if (num_buff >= 0 && num_buff <= 100){
            status_n++;
            this->voice_volume = (unsigned int) num_buff;
          } 
          json_object_clear(volume_obj);
        }
        rate_obj = json_object_get(voice_obj, "rate");
        if (json_is_integer(rate_obj)){
          num_buff = 0;
          num_buff = (int) json_integer_value(rate_obj);
          if (num_buff >= -10 && num_buff <= 10){
            status_n++;
            this->voice_rate = (int) num_buff;
          }
          json_object_clear(rate_obj);
        }
        json_object_clear(voice_obj);
      }
      link_obj = json_object_get(json, "link");
      if (json_is_object(link_obj)){
        host_obj = json_object_get(link_obj, "host");
        if (json_is_string(host_obj)){
          this->link_host = strcopy(json_string_value(host_obj));
          if (strlen(this->link_host) > 0) status_n++;
          json_object_clear(host_obj);
        }
        port_obj = json_object_get(link_obj, "port");
        if (json_is_integer(port_obj)){
          num_buff = 0;
          num_buff = (int) json_integer_value(port_obj);
          if (num_buff >= 0 && num_buff <= 0xFFFF){
            status_n++;
            this->link_port = (unsigned int) num_buff;
          }
          json_object_clear(port_obj);
        }
        ssl_obj = json_object_get(link_obj, "ssl");
        if (json_is_boolean(ssl_obj)){
          if (json_is_true(ssl_obj)){
            this->link_ssl = true;
          } else {
            this->link_ssl = false;
          }
          status_n++;
          json_object_clear(ssl_obj);
        }
        json_object_clear(link_obj);
      }
      json_object_clear(json);
    }
  }
  if (status_n == 6) this->parse_status = true;
}

char *Configer::getVoiceName(){
  return strcopy(this->voice_name);
}

unsigned int Configer::getVoiceVolume(){
  return this->voice_volume;
}

int Configer::getVoiceRate(){
  return this->voice_rate;
}

char *Configer::getLinkHost(){
  return strcopy(this->link_host);
}

unsigned int Configer::getLinkPort(){
  return this->link_port;
}

bool Configer::getLinkSSL(){
  return this->link_ssl;
}

bool Configer::getStatus(){
  return this->status;
}
