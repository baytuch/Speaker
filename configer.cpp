
#include <iostream>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <jansson.h>
#include "configer.h"
#ifndef SP_CONF
#define SP_CONF "config.json"
#endif

Configer::Configer(){
  this->load_status = false;
  this->parse_status = false;
  this->status = false;
  this->voice_name = "";
  this->buff = "";
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
    this->buff = (char*) malloc(buff_size * sizeof(char));
    memset(this->buff, 0x00, buff_size);
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
  json_t *name_obj;
  json_t *volume_obj;
  json_t *rate_obj;
  char *str_buff;
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
          str_buff = (char*) json_string_value(name_obj);
          this->voice_name = (char*) malloc((strlen(str_buff) + 1) * sizeof(char));
          strncpy(this->voice_name, str_buff, (strlen(str_buff) + 1) * sizeof(char));
          if (strlen(str_buff) > 0) status_n++;
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
      json_object_clear(json);
    }
  }
  if (status_n == 3) this->parse_status = true;
}

bool Configer::getStatus(){
  return this->status;
}

char *Configer::getVoiceName(){
  char *name;
  if (this->voice_name != NULL){
    name = (char*) malloc((strlen(this->voice_name) + 1) * sizeof(char));
    strncpy(name, this->voice_name, (strlen(this->voice_name) + 1) * sizeof(char));
  }
  return name;
}

unsigned int Configer::getVoiceVolume(){
  return this->voice_volume;
}

int Configer::getVoiceRate(){
  return this->voice_rate;
}
