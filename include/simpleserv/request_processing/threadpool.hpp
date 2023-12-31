#ifndef WEBSERVER_THREADPOOL_HPP
#define WEBSERVER_THREADPOOL_HPP

#include <simpleserv/request_processing/connection.hpp>
#include <simpleserv/utility/lock_free_queue.hpp>

#include <cstdint>
#include <thread>

namespace request_processing {
	template <uint32_t num_threads, typename ConnectionObject, IConnectionHandler<ConnectionObject, 100> C>
	class Threadpool {
	private:
		LockFreeQueue<ConnectionObject, 100> _queue;
		std::array<std::thread, num_threads> _threads;
		std::array<C, num_threads> _handlers;

	public:
		Threadpool() : _queue(LockFreeQueue<ConnectionObject, 100>()) {
			for (unsigned int i = 0; i < num_threads; i++) {
				_handlers.at(i) = std::move(C(&_queue));

				_threads.at(i) = std::thread(&C::Start, &_handlers.at(i));
			}
		}

		~Threadpool() = default;

		Threadpool(Threadpool&&) = default;
		Threadpool& operator=(Threadpool&&) = default;
		Threadpool(Threadpool&) = delete;
		Threadpool& operator=(Threadpool&) = delete;


		void Start() {}
		void add_connection(ConnectionObject obj) {
			_queue->push(obj);
		}

		void join() {
			for (auto& thread : _threads) {
				thread.join();
			}
		}

		void add_task(ConnectionObject obj) {
			_queue.push(obj);
		}
	};
}; // namespace request_processing

#endif // WEBSERVER_THREADPOOL_HPP