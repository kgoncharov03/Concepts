#pragma once


#include <atomic>
#include <condition_variable>
#include <functional>
#include <future>
#include <iostream>
#include <mutex>
#include <queue>
#include <vector>

template <class T, class Container = std::deque<T>>
class BlockingQueue { 
private:
	std::mutex mutex_;
	size_t capacity_;
	Container queue_;
	bool state_;
	std::condition_variable cv_is_empty_;
	std::condition_variable cv_is_full_;
public:
	explicit BlockingQueue(const size_t& capacity);
	void Put(T&& element);
	bool Get(T& result);
	void Shutdown();
	bool empty() const;
	bool full() const;
};

template <class T, class Container>
BlockingQueue<T, Container>::BlockingQueue(const size_t &capacity) {
	state_ = true;
	capacity_ = capacity;
	if (capacity == 0) {
		throw std::exception();
	}
}

template <class T, class Container>
bool BlockingQueue<T, Container>::empty() const {
	return queue_.empty();
}

template <class T, class Container>
bool BlockingQueue<T, Container>::full() const {
	return (queue_.size() == capacity_);
}

template <class T, class Container>
void BlockingQueue<T, Container>::Put(T&& element) {
	std::unique_lock<std::mutex> lock(mutex_);
	if (!state_) {
		throw std::exception();
	}
	cv_is_full_.wait(lock, [this] { return !state_ || !full(); });
	if (!state_) {
		throw std::exception();
	}
	queue_.push_back(std::move(element));
	cv_is_empty_.notify_one();
}

template <class T, class Container>
bool BlockingQueue<T, Container>::Get(T &result) {
	std::unique_lock<std::mutex> lock(mutex_);
	cv_is_empty_.wait(lock, [this] { return !state_ || !empty(); });
	if (empty() && !state_) {
		return false;
	}
	result = std::move(queue_.front());
	queue_.pop_front();
	cv_is_full_.notify_one();
	return true;
}

template <class T, class Container>
void BlockingQueue<T, Container>::Shutdown() {
	std::unique_lock<std::mutex> lock(mutex_);
	state_ = false;
	cv_is_empty_.notify_all();
	cv_is_full_.notify_all();
}



template <class T>
class ThreadPool {
public:

	explicit ThreadPool(const size_t num_threads) : state_(true), workers_(num_threads), num_free_threads_(default_num_threads()),
		queue_(num_threads) {
		for (auto& workers_it : workers_) {
			auto worker_function = [this]() {
				std::packaged_task<T()> task;
				while (queue_.Get(task)) {
					num_free_threads_.fetch_sub(1);
					task();
					num_free_threads_.fetch_add(1);
				}
			};
			workers_it = std::thread(worker_function);
		}
	}

	ThreadPool() : ThreadPool(default_num_threads()) {}

	std::future<T> Submit(std::function<T()> task) {
		std::packaged_task<T()> curr_task(task);
		std::future<T> result = curr_task.get_future();
		queue_.Put(std::move(curr_task));
		return result;
	}

	void Shutdown() {
		state_.store(false);
		queue_.Shutdown();
		for (auto& workers_it : workers_)
			workers_it.join();
	}

	std::atomic<size_t> num_free_threads_;

	~ThreadPool() {
		if (state_) {
			Shutdown();
		}
	}
private:
	std::atomic<bool> state_;
	std::vector<std::thread> workers_;
	BlockingQueue<std::packaged_task<T()>> queue_;
	size_t default_num_threads() {
		size_t result = std::thread::hardware_concurrency();
		if (result) {
			return result;
		}
		return 4;
	}

};

template <class T>
class Async {
public:
	static std::future<T> async(std::function<T()> func, bool mode) {
		if (mode && pool.num_free_threads_.load() > 0) {
			return pool.Submit(func);
		}
		else {
			std::promise<T> promise;
			try {
				promise.set_value(func());
			}
			catch (std::exception_ptr excep) {
				promise.set_exception(excep);
			}
			return promise.get_future();
		}
	}
private:
	static ThreadPool<T> pool;
};

