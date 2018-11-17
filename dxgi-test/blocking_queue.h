#ifndef BLOCKING_QUEUE_H__
#define BLOCKING_QUEUE_H__

#include <mutex>
#include <condition_variable>
#include <deque>

template <class T> 
class BlockingQueue
{
private:
	std::mutex mutex_;
	std::condition_variable condition_;
	std::deque<T> queue_;
public:
	void put(const T& value)
	{
		{
			std::unique_lock<std::mutex> lock(mutex_);
			queue_.push_back(value);
		}
		condition_.notify_one();
	}

	T take()
	{
		std::unique_lock<std::mutex> lock(mutex_);
		condition_.wait(lock, [=] {return !this->queue_.empty(); });
		T rc(std::move(queue_.front()));
		queue_.pop_front();
		return rc;
	}
};
#endif // BLOCKING_QUEUE_H__

