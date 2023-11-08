#ifndef WEBSERVER_LOCK_FREE_QUEUE_HPP
#define WEBSERVER_LOCK_FREE_QUEUE_HPP

#include <atomic>
#include <array>
#include <cstddef>
#include <cstdint>
#include <iostream>
#include <memory>
#include <optional>
#include <stdexcept>
#include <type_traits>
#include <utility>

// TODO(Hunor): Investigate whether the small integer optimisations are worthwhile

#define ENABLE_SMALL_INTEGER_OPTIMISATIONS

constexpr size_t SMALL_INTEGER_OPTIMISATION_LIMIT = 100; 

template <typename T, uint32_t capacity>
class LockFreeQueue {
private:
	std::array<T, capacity> arr_;
	std::atomic<uint32_t> head_;
	std::atomic<uint32_t> tail_;
	std::atomic<uint32_t> size_;

// TODO: test efficacy of this optimisation
#ifdef ENABLE_SMALL_INTEGER_OPTIMISATIONS
	using next_array = std::conditional_t<capacity <= SMALL_INTEGER_OPTIMISATION_LIMIT, std::array<uint32_t, capacity>, void>;

	next_array next_arr;
#endif

public:
	LockFreeQueue() {
		#ifdef ENABLE_SMALL_INTEGER_OPTIMISATIONS
		if constexpr (capacity <= SMALL_INTEGER_OPTIMISATION_LIMIT) {
			for(uint32_t i = 0; i < capacity; i++) {
				next_arr[i] = i + 1;
			}
			next_arr[capacity - 1] = 0;
		}
		#endif
	};

	LockFreeQueue(LockFreeQueue&&) = default;
	LockFreeQueue& operator=(LockFreeQueue&&) = delete;
	LockFreeQueue(LockFreeQueue&) = delete;
	LockFreeQueue& operator=(LockFreeQueue&) = delete;

	~LockFreeQueue() = default;

	bool push(T&& elem) {
		uint32_t tail = tail_.load(std::memory_order_acquire);
		uint32_t next_tail = next(tail);

		arr_[tail] = std::move(elem);
		tail_.store(next_tail, std::memory_order_release);
		size_.fetch_add(1, std::memory_order_release);

		return true;
	}

	bool push(const T& elem) {
		const uint32_t tail = tail_.load(std::memory_order_acquire);
		const uint32_t next_tail = next(tail);

		arr_[tail] = elem;
		tail_.store(next_tail, std::memory_order_release);
		size_.fetch_add(1, std::memory_order_release);

		return true;
	}

	std::optional<T> pop() {
		uint32_t head = head_.load(std::memory_order_relaxed);

		// Boundary check
		if (head == tail_.load(std::memory_order_acquire)) {
			return std::nullopt;
		}
		
		// Attempt to access exclusive access to the current head
		if(!head_.compare_exchange_weak(head, next(head), std::memory_order_release)) {
			return std::nullopt;
		}
		
		// Copy out the data and update counters
		T result = std::move(arr_[head]);
		size_.fetch_sub(1, std::memory_order_release);
		return result;
	}

	[[nodiscard]] bool empty() const {
		return head_.load(std::memory_order_acquire) == tail_.load(std::memory_order_acquire);
	}

	[[nodiscard]] bool full() const {
		uint32_t tail = tail_.load(std::memory_order_acquire);
		return next(tail) == head_.load(std::memory_order_acquire);
	}

	LockFreeQueue& createRef() {
		return &this;
	}

private:
	[[nodiscard]] constexpr uint32_t next(uint32_t current) const {
		#ifdef ENABLE_SMALL_INTEGER_OPTIMISATIONS
		if constexpr (capacity < SMALL_INTEGER_OPTIMISATION_LIMIT) {
			return next_arr[current];
		} else {
			return (current + 1) % capacity;
		}
		#else
			return (current + 1) % capacity;
		#endif
	}
};

#endif // WEBSERVER_LOCK_FREE_QUEUE_HPP
