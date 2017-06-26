#include "logging.h"

#include <sys/stat.h>
#include <stdarg.h>

using namespace std;

Log::LogMsg::LogMsg(std::string& msg){
  this->message = msg;
}

ostream& operator<<(ostream& logstream, const Log::LogMsg& msg){
  return logstream << msg.message;
}

Log::Log(): msg_queue(), trans_mutex(), run_cv(){
  is_open = false;
}

Log& Log::init(){
  static Log instance;
  return instance;
}

void Log::thread_handle(){
  while(1){
    unique_lock<mutex> flck(this->trans_mutex);
    while(msg_queue.empty()){
      if (!is_open){
        return;
      }
      this->run_cv.wait(flck);
    }
    shared_ptr<LogMsg> msg = msg_queue.front();
    fout << *msg; // << std::endl;
    msg_queue.pop();
  }
}

void Log::open(){
  struct stat buf;
  fout.open(logfile, ofstream::out | ios::app);
  if(fout.fail()){
    throw std::ios_base::failure("Error opening log file.");
  }
  is_open = true;
  thread t(&Log::thread_handle, this);
  exec_thread.swap(t);
}

void Log::write(const std::string& format, ...){
  char buffer[format.size() + 1];
  va_list args;
  va_start(args, format);
  vsprintf(buffer, format.c_str(), args);
  std::string message(buffer);
  //printf("%s\n", message.c_str());
  shared_ptr<LogMsg> msg = make_shared<LogMsg>(message);
  unique_lock<mutex> flck(trans_mutex);
  if(!is_open){
    flck.unlock();
    throw std::runtime_error("The logger received a message but the log file is not open.");
  }
  bool queue_empty = msg_queue.empty();
  msg_queue.push(msg);
  if(queue_empty){
    run_cv.notify_one();
  }
}

void Log::close(){
  unique_lock<mutex> flck(trans_mutex);
  if(!is_open)
    return;
  is_open = false;
  flck.unlock();
  run_cv.notify_one();
  exec_thread.join();
  fout.close();
}

extern std::string currentTimestamp() {
  const unsigned int time_size = 256;
  char time_buffer[time_size];
  time_t now = time(NULL);
  struct tm *tmp = localtime(&now);
  strftime(time_buffer, time_size, "[%m/%d/%y %H:%M:%S]", tmp);
  return std::string(time_buffer);
}
