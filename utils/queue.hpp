#ifndef QUEUE_H
#define QUEUE_H

#include <iostream>
#include <mutex>
#include <condition_variable>
#include <deque>
#include <string>
//
// blocking queue

template <typename T>
class syque
{
private:
  std::mutex              d_mutex;
  std::condition_variable d_condition;
  std::deque<T>           d_queue;
  bool                    eos = false; 
  bool                    balance = false;
public:

  syque(std::string s) { std::cout << "Created " << s << " queue " << std::endl;  }
  syque() {}
  
  void push(T&& value) {
    {
      std::unique_lock<std::mutex> lock(this->d_mutex);
      d_queue.push_front(std::forward<T>(value));
    }
    this->d_condition.notify_one();
  }

  T pop() {
    std::unique_lock<std::mutex> lock(this->d_mutex);
    this->d_condition.wait(lock, [=]{ return !(this->d_queue.empty()); });
    T rc(std::move(this->d_queue.back()));
    this->d_queue.pop_back();
    return rc;
  }

};

#endif