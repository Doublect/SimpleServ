#ifndef WEBSERVER_CONNECTION_HANDLER_HPP
#define WEBSERVER_CONNECTION_HANDLER_HPP

#include <simpleserv/debug_flags.hpp>

#include <simpleserv/http/http_handler.hpp>
#include <simpleserv/server/handler.hpp>
#include <simpleserv/utility/lock_free_queue.hpp>
#include "connection.hpp"

#include <cassert>
#include <iostream>
#include <stdexcept>
#include <thread>

namespace request_processing {	
	template <server::IHandler Handler, typename ConnectionObject, class Sender, ICloser<ConnectionObject> Closer>
	class ThreadpoolConnectionHandler {
	private:

		LockFreeQueue<ConnectionObject, 100> *_queue;
		bool ready;

	public:
		ThreadpoolConnectionHandler() : ready(false) {}
		ThreadpoolConnectionHandler(LockFreeQueue<ConnectionObject, 100> *queue) : _queue(queue), ready(true) {}

		ThreadpoolConnectionHandler(ThreadpoolConnectionHandler&&) = default;
		ThreadpoolConnectionHandler& operator=(ThreadpoolConnectionHandler&&) = default;
		ThreadpoolConnectionHandler(ThreadpoolConnectionHandler&) = default;
		ThreadpoolConnectionHandler& operator=(ThreadpoolConnectionHandler&) = default;

		void Start() {
			assert(_queue != nullptr && "Threadpool worker lacks a job queue.");

			if(!ready) {
				throw std::runtime_error("Threadpool worker lacks a job queue to take jobs from.");
			}

			if constexpr (_debug && (_debug_threadpool_worker)) {
				std::cout << "Starting threadpool connection handler..." << std::endl;
			}
			while (true) {
				auto obj = _queue->pop();
				if (obj.has_value()) {
					handle_connection(obj.value());
				}
			}
		}

		void handle_connection(ConnectionObject co) {
				if constexpr (_debug && (_debug_threadpool_worker)) {
					std::cout << "Received HTTP request: " << co.request.length() << " on thread: " << std::this_thread::get_id() << std::endl;
				}
				auto response = Handler::handle(co.request);
				
				if (response.has_value()) {
					Sender::SendMsg(co, response.value());
				}

				Closer::CloseConnection(co);
		}
	};
}; // namespace request_processing

#endif // WEBSERVER_CONNECTION_HANDLER_HPP