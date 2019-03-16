
#ifndef LOGGER_H_INCLUDED
#define LOGGER_H_INCLUDED

#ifndef SP_LOG
#define SP_LOG "voice.log"
#endif

class Logger {

  public:

    Logger();
    Logger(const char *srv_name, const char *log_file = SP_LOG);
    ~Logger();
    void operator= (const Logger &other);
    void operator<< (const char *msg);

  private:
    char *srv_name;
    char *log_file;
    char *msg;
    bool default_mode;
    static const unsigned int msg_length;
    static bool update_lock;
    void add_date();
    void add_name();
    void write_msg();
    void show_msg();
    void do_logger(const char *msg);

};

#endif // LOGGER_H_INCLUDED
