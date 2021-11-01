#pragma once
#include <thread>
#include <condition_variable>
#include <list>
#include <functional>

class Worker
{
public:
	Worker(bool start) : mRunning(start) { if(start) private_start(); }
	Worker() : mRunning(false) {}
	~Worker() { stop(); }

	template<typename... Args>
	void push_task(Args&&... args)
	{
		{
			std::lock_guard<std::mutex> lk(mMutex);
			mQueue.push_back(std::bind(std::forward<Args>(args)...));
		}

		mCondition.notify_all();
	}

	void start()
	{
		{
			std::lock_guard<std::mutex> lk(mMutex);
			if(mRunning == true) return;
			mRunning = true;
		}

		private_start();
	}

	void stop()
	{
		{
			std::lock_guard<std::mutex> lk(mMutex);
			if(mRunning == false) return;
			mRunning = false;
		}

		mCondition.notify_all();
		mThread.join();
	}

private:
	void private_start()
	{
		mThread = std::thread([this]
		{
			for(;;)
			{
				decltype(mQueue) local_queue;
				{
					std::unique_lock<std::mutex> lk(mMutex);
					mCondition.wait(lk, [&] { return !mQueue.empty() + !mRunning; });

					if(!mRunning)
					{
						for(auto& func : mQueue)
							func();

						mQueue.clear();
						return;
					}

					std::swap(mQueue, local_queue);
				}

				for(auto& func : local_queue)
					func();
			}
		});
	}

private:
	std::condition_variable mCondition;
	std::list<std::function<void()>> mQueue;
	std::mutex mMutex;
	std::thread mThread;
	bool mRunning = false;
};