#ifndef _LOGGING_H_
#define _LOGGING_H_ 1

#include <fstream>
#include <queue>
#include <thread>
#include <mutex>
#include <condition_variable>

#define LOGFILE "dnsseed.log"

static const char* logfile = LOGFILE;
std::string currentTimestamp();

class Log {
    public:
        static Log& init();
        void open();
        void write(const std::string& format, ...);
        void close();

    private:
        bool is_open;
        std::ofstream fout;
        std::mutex trans_mutex;
        std::condition_variable run_cv;
        std::thread exec_thread;

        class LogMsg {
            public:
                LogMsg(std::string&);
                friend std::ostream& operator<<(std::ostream&, const LogMsg&);
	    private:
                std::string message;
        };

        std::queue<std::shared_ptr<LogMsg>> msg_queue;
        Log();
        Log(Log const&){};
        Log& operator=(Log const&);
        void thread_handle();
        friend std::ostream& operator<<(std::ostream&, const LogMsg&);
};

#endif
