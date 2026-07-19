#include <thread>
#include <stdexcept>
#include <future>
#include <functional>

#include "BlockingQueue.h"

class ThreadPool
{
public:
	explicit ThreadPool(size_t numThreads)
	{
		if (numThreads == 0)
		{
			throw std::invalid_argument("ThreadPool: 0 threads");
		}
		
		for (size_t i = 0; i < numThreads; ++i)
		{
			workers_.emplace_back([this]() {
				while (auto task = queue_.pop())
				{
					(*task)();
				}
			});
		}
	}
	
	~ThreadPool()
	{
		queue_.shutdown();
		
		for (auto& worker : workers_)
		{
			worker.join();
		}
	}
	
	template <typename F>
	bool submit(F&& task)
	{
		return queue_.push(std::function<void()>(std::forward<F>(task)));
	}
	
private:
	BlockingQueue<std::function<void()>> queue_;
	std::vector<std::thread> workers_;
};
