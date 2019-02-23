
#include <iostream>
#include <wchar.h>
#include <string>
#include <vector>
#include <atlbase.h>
#include <sapi.h>
#include <sphelper.h>
#include "speak_worker.h"


Speak_worker::Speak_worker(){
  this->status = false;
  CoInitialize(NULL);
  HRESULT hr = CoCreateInstance(CLSID_SpVoice, NULL, CLSCTX_ALL, IID_ISpVoice, (void **) &this->voice);
  if (SUCCEEDED(hr)){
    this->voice->SetOutput(NULL, TRUE);
    SpEnumTokens(SPCAT_VOICES, L"", L"", &this->voices_info);
    this->status = true;
  }
}

Speak_worker::~Speak_worker(){
  CoUninitialize();
}

void Speak_worker::say(){
  this->status = false;
  HRESULT hr = this->voice->Speak(L"Hello world", SPF_ASYNC | SPF_IS_XML, NULL);
  if (SUCCEEDED(hr)){
    this->voice->WaitUntilDone(INFINITE);
    this->status = true;
  }
}

void Speak_worker::getVoices(std::vector<std::string> &voices){
  const ULONG key_n_max = 16;
  const LPWSTR target_key_name = L"Attributes";
  const LPWSTR value_name = L"Name";
  ULONG voice_len = 0;
  ULONG key_n = 0;
  LPWSTR key_name = NULL;
  LPWSTR voice_name = NULL;
  CComPtr<ISpObjectToken> voice_info_obj = NULL;
  CComPtr<ISpDataKey> voice_info_data = NULL;
  this->status = false;
  voices.clear();
  this->voices_info->GetCount(&voice_len);
  if (voice_len > 0){
    voices.reserve(voice_len);
    for (ULONG n = 0; n < voice_len; n++){
      this->voices_info->Item(n, &voice_info_obj);
      while (true) {
        HRESULT res = voice_info_obj->EnumKeys(key_n, &key_name);
        if (SUCCEEDED(res) && key_n < key_n_max){
          if (wcscmp(key_name, target_key_name) == 0) break;
	    } else {
          break;
	    }
        key_n++;
	  }
      voice_info_obj->OpenKey(key_name, &voice_info_data);
      voice_info_data->GetStringValue(value_name, &voice_name);
      voices.push_back(std::string(CW2A(voice_name)));
      voice_info_obj.Release();
      voice_info_data.Release();
	}
  }
  if (voices.size() == voice_len) this->status = true;
}

bool Speak_worker::getStatus(){
  return this->status;
}