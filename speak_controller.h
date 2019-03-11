
#ifndef SPEAK_CONTROLLER_H_INCLUDED
#define SPEAK_CONTROLLER_H_INCLUDED

#include <pthread.h>
#include "speak_worker.h"

class Speak_controller {

  public:
    Speak_controller();
    ~Speak_controller();
    bool tell(const char *oration);
    void stop();

  private:
    char *loop_buffer;
    static const unsigned int say_length;
    static const unsigned int say_size;
    Speak_worker worker;
    bool worker_run;
    bool worker_is_stop;
    pthread_t loop_tid;
    unsigned int push_n;
    unsigned int pull_n;
    bool push_lock;
    bool buffer_overflow;
    void loop();
    static void *init_loop(void *vptr_args);
    void worker_init();
    void worker_stop();
    void worker_do();

};

#endif // SPEAK_CONTROLLER_H_INCLUDED
