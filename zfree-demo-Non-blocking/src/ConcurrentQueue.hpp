#include <queue>
#include <string>
#include <pthread.h>
#include <mutex>

class ConcurrentQueue {
public:
  ConcurrentQueue() = default;

  void push(std::string &hashValue) 
  {
    std::lock_guard<std::mutex> guard(queueMutex);
    hashQueue.push(hashValue);
  }

  std::string pop()
  {
    std::lock_guard<std::mutex> guard(queueMutex);
    std::string hashValue = hashQueue.front();
    hashQueue.pop();
    return hashValue;
  }

  bool empty() 
  {
    return hashQueue.empty();
  }

  size_t size() 
  {
    return hashQueue.size();
  }

private:
  queue<std::string> hashQueue;  
  std::mutex queueMutex;
};