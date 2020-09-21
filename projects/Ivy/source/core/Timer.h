#pragma once
#include <chrono>

namespace Ivy
{
	class Timer
	{
	public:
		void Start()
		{
			mStartTime = std::chrono::system_clock::now();
			mbRunning = true;
		}

		void Stop()
		{
			mEndTime = std::chrono::system_clock::now();
			mbRunning = false;
		}

		double ElapsedMilliseconds()
		{
			std::chrono::time_point<std::chrono::system_clock> endTime;

			if (mbRunning)
			{
				endTime = std::chrono::system_clock::now();
			}
			else
			{
				endTime = mEndTime;
			}

			return std::chrono::duration_cast<std::chrono::milliseconds>(endTime - mStartTime).count();
		}

		double ElapsedSeconds()
		{
			return ElapsedMilliseconds() / 1000.0;
		}

	private:
		std::chrono::time_point<std::chrono::system_clock> mStartTime;
		std::chrono::time_point<std::chrono::system_clock> mEndTime;
		bool                                               mbRunning = false;
	};
}