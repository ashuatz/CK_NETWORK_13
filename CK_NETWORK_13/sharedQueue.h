#pragma once

#include <queue>
#include <mutex>
#include <condition_variable>
//
//template <typename T>
//class SharedQueue
//{
//public:
//	SharedQueue();
//	~SharedQueue();
//
//	T& front();
//	void pop_front();
//	T& dequeue();
//
//	void push_back(const T& item);
//	void push_back(T&& item);
//
//	int size();
//	bool empty();
//
//private:
//	std::deque<T> queue_;
//	std::mutex mutex_;
//	std::condition_variable condition_;
//};
//
//template <typename T>
//SharedQueue<T>::SharedQueue() {}
//
//template <typename T>
//SharedQueue<T>::~SharedQueue() {}
//
//template <typename T>
//T& SharedQueue<T>::front()
//{
//	std::unique_lock<std::mutex> mlock(mutex_);
//	while (queue_.empty())
//	{
//		condition_.wait(mlock);
//	}
//	return queue_.front();
//}
//
//template <typename T>
//void SharedQueue<T>::pop_front()
//{
//	std::unique_lock<std::mutex> mlock(mutex_);
//	while (queue_.empty())
//	{
//		condition_.wait(mlock);
//	}
//	queue_.pop_front();
//}
//
//template<typename T>
//inline T & SharedQueue<T>::dequeue()
//{
//	std::unique_lock<std::mutex> mlock(mutex_);
//	while (queue_.empty())
//	{
//		condition_.wait(mlock);
//	}
//
//	auto temp = queue_.front();
//	queue_.pop_front();
//	return temp;
//}
//
//template <typename T>
//void SharedQueue<T>::push_back(const T& item)
//{
//	std::unique_lock<std::mutex> mlock(mutex_);
//	queue_.push_back(item);
//	mlock.unlock();     // unlock before notificiation to minimize mutex con
//	condition_.notify_one(); // notify one waiting thread
//
//}
//
//template <typename T>
//void SharedQueue<T>::push_back(T&& item)
//{
//	std::unique_lock<std::mutex> mlock(mutex_);
//	queue_.push_back(std::move(item));
//	mlock.unlock();     // unlock before notificiation to minimize mutex con
//	condition_.notify_one(); // notify one waiting thread
//
//}
//
//template <typename T>
//int SharedQueue<T>::size()
//{
//	std::unique_lock<std::mutex> mlock(mutex_);
//	int size = queue_.size();
//	mlock.unlock();
//	return size;
//}
//
//template<typename T>
//inline bool SharedQueue<T>::empty()
//{
//	std::unique_lock<std::mutex> mlock(mutex_);
//
//	return size() == 0;
//}
