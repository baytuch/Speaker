
#include <iostream>
#include <string>
#include <vector>
#include <jansson.h>
#include "utils.h"
#include "semantic.h"


Semantic::Semantic(const char *smt_db){
  this->load_status = false;
  this->parse_status = false;
  this->status = false;
  this->pattern_db.clear();
  this->load(smt_db);
  if (this->load_status){
    this->parse();
  }
  if (this->pattern_db.size() > 0){
    this->status = true;
  }
}

Semantic::~Semantic(){
}

void Semantic::load(const char *smt_db){
  const unsigned int buff_size = 65536;
  FILE *fd;
  int c;
  this->load_status = false;
  fd = fopen(smt_db, "r");
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

void Semantic::parse(){
  json_t *json;
  json_error_t error;
  json_t *pattern_data_obj;
  json_t *pattern_obj;
  json_t *code_obj;
  size_t db_size = 0;
  this->parse_status = false;
  json = json_loads(this->buff, 0, &error);
  free(this->buff);
  if(json){
    if (json_is_array(json)){
      db_size = json_array_size(json);
      if (db_size > 0){
        this->pattern_db.clear();
        this->pattern_db.reserve(db_size);
        this->pattern_db.resize(db_size);
        for (unsigned int n = 0; n < db_size; n++){
          pattern_data_obj = json_array_get(json, n);
          if (json_is_object(pattern_data_obj)){
            pattern_obj = json_object_get(pattern_data_obj, "pattern");
            if (json_is_string(pattern_obj)){
              this->pattern_db[n].pattern = std::string(json_string_value(pattern_obj));
              json_object_clear(pattern_obj);
            }
            code_obj = json_object_get(pattern_data_obj, "code");
            if (json_is_string(code_obj)){
              this->pattern_db[n].code = std::string(json_string_value(code_obj));
              json_object_clear(code_obj);
            }
            json_object_clear(pattern_data_obj);
          }
        }
        this->parse_status = true;
      }
      json_object_clear(json);
    }
  }
}

char *Semantic::compiler(const char *code){
  std::string str_res = "";
  std::string str_code = std::string(code);
  size_t code_id = 0;
  bool search_status = false;
  for (unsigned int n = 0; n < this->pattern_db.size(); n++){
    if (str_code.find(this->pattern_db[n].code) != std::string::npos){
      code_id = (size_t) n;
      search_status = true;
    }
  }
  if (search_status){
    str_res = this->pattern_db[code_id].pattern;
  } else {
    str_res = this->pattern_db[0].pattern;
  }
  return strcopy(str_res.c_str());
}

bool Semantic::getStatus(){
  return this->status;
}
