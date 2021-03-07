#pragma once
#include <thread>
#include <condition_variable>
#include <list>

class Worker
{
public:
	Worker(bool start) : m_Running(start) { if(start) private_start(); }
	Worker() : m_Running(false) {}
	~Worker() { stop(); }

	template<typename... Args>
	void push_task(Args&&... args)
	{
		{
			std::lock_guard<std::mutex> lk(m_Mutex);
			m_Queue.push_back(std::bind(std::forward<Args>(args)...));
		}

		m_Condition.notify_all();
	}

	void start()
	{
		{
			std::lock_guard<std::mutex> lk(m_Mutex);
			if(m_Running == true) return;
			m_Running = true;
		}

		private_start();
	}

	void stop()
	{
		{
			std::lock_guard<std::mutex> lk(m_Mutex);
			if(m_Running == false) return;
			m_Running = false;
		}

		m_Condition.notify_all();
		m_Thread.join();
	}

private:
	void private_start()
	{
		m_Thread = std::thread([this]
		{
			for(;;)
			{
				decltype(m_Queue) local_queue;
				{
					std::unique_lock<std::mutex> lk(m_Mutex);
					m_Condition.wait(lk, [&] { return !m_Queue.empty() + !m_Running; });

					if(!m_Running)
					{
						for(auto& func : m_Queue)
							func();

						m_Queue.clear();
						return;
					}

					std::swap(m_Queue, local_queue);
				}

				for(auto& func : local_queue)
					func();
			}
		});
	}

private:
	std::condition_variable m_Condition;
	std::list<std::function<void()>> m_Queue;
	std::mutex m_Mutex;
	std::thread m_Thread;
	bool m_Running = false;
};