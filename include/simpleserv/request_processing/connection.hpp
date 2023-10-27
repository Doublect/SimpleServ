#ifndef WEBSERVER_CONNECTION_HPP
#define WEBSERVER_CONNECTION_HPP

#include <simpleserv/utility/lock_free_queue.hpp>
#include <simpleserv/http/message.hpp>

namespace request_processing {
	template <class T, typename ConnectionObject>
	concept ICloser = requires(T t, ConnectionObject &co) {
		{ T::CloseConnection(co) } -> std::same_as<void>;
	};

	template <class T, typename ConnectionObject, unsigned int capacity>
	concept IConnectionHandler = requires(T t, ConnectionObject co, LockFreeQueue<ConnectionObject, capacity> *queue) {
		{ t.Start() } -> std::same_as<void>;
		{ t.handle_connection(co) } -> std::same_as<void>;
	};
} // namespace request_processing

#endif // WEBSERVER_CONNECTION_HPP