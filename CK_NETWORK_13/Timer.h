#pragma once
#include "singleton.h"
#include <Windows.h>
#include <stdexcept>

class PerformanceCounter : public Singleton<PerformanceCounter>
{
private:
	double PCFreq = 0.0;
	__int64 CounterStart = 0;

public:
	void StartCounter()
	{
		LARGE_INTEGER li;
		if (!QueryPerformanceFrequency(&li))
			throw std::bad_exception();

		PCFreq = double(li.QuadPart) * 0.001;

		QueryPerformanceCounter(&li);
		CounterStart = li.QuadPart;
	}

	double GetCounter()
	{
		LARGE_INTEGER li;
		QueryPerformanceCounter(&li);
		return double(li.QuadPart - CounterStart) / PCFreq;
	}
};

class Time : public Singleton<Time>
{
public:
private:
	__int64 startTime;
	__int64 currentTime;
	__int64 lastSecond;

	int cachedFrameRate;
	float cachedDeltaTime;

public:

	Time()
	{
		cachedDeltaTime = 0;
		cachedFrameRate = 0;
	}

	float GetDeltaTime() { return cachedDeltaTime; }
	int GetFrame() { return cachedFrameRate; }

	void CheckCounter()
	{
		auto counter = PerformanceCounter::GetInstance().GetCounter();
		cachedDeltaTime = counter != INFINITY ? counter : 0;
		PerformanceCounter::GetInstance().StartCounter();

		cachedFrameRate = (int)(1000 / cachedDeltaTime);
		currentTime += cachedDeltaTime;

		if (((currentTime - startTime) % 10) != lastSecond)
		{
			lastSecond = (currentTime - startTime) % 10;
		}
	}

private:
protected:

};


#include <thread>
#include <chrono>
#include <functional>

template <typename T>
class Timer
{
private:
	bool m_running = false;
	long m_interval;

	std::thread m_thread;
	std::function<T> m_func;

	bool m_initialized;
public:
	Timer() : m_initialized(false) {}

	Timer(const std::function<T>& func, const long &interval)
	{
		m_func = func;
		m_interval = interval;

		m_initialized = true;
	}

	void Initialize(const std::function<T>& func, const long &interval)
	{
		if (m_initialized)
		{
			stop();
		}

		m_func = func;
		m_interval = interval;

		m_initialized = true;
	}

	void start()
	{
		m_running = true;
		m_thread = std::thread([&]()
		{
			while (m_running)
			{
				auto delta = std::chrono::steady_clock::now() + std::chrono::milliseconds(m_interval);

				//humm....
				m_func();
				std::this_thread::sleep_until(delta);
			}
		});
		m_thread.detach();
	}

	void stop()
	{
		m_running = false;
		m_thread.~thread();
	}

	void restart()
	{
		stop();
		start();
	}

	Timer& setFunc(std::function<void(void)> func)
	{
		m_func = func;
		return *this;
	}

	Timer& setInterval(const long &interval)
	{
		m_interval = interval;
		return *this;
	}

	bool isInitialized() const
	{
		return m_initialized;
	}
	bool isRunning() const
	{
		return m_running;
	}

	long getInterval() const
	{
		return m_interval;
	}

	~Timer()
	{
		stop();
	}
};