
#ifndef SPEAK_CONTROLLER_H_INCLUDED
#define SPEAK_CONTROLLER_H_INCLUDED

#include <pthread.h>

class Speak_controller {

  public:
    Speak_controller();
    ~Speak_controller();

  private:
    char *loop_buffer;
    static const unsigned int say_length;
    static const unsigned int say_size;
    pthread_t loop_tid;
    void loop();
    static void *init_loop(void *vptr_args);

};

#endif // SPEAK_CONTROLLER_H_INCLUDED
