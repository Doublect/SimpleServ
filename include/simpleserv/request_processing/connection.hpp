#ifndef WEBSERVER_CONNECTION_HPP
#define WEBSERVER_CONNECTION_HPP

#include <simpleserv/utility/lock_free_queue.hpp>
#include <simpleserv/http/message.hpp>

namespace request_processing {
	template <class T, typename ConnectionObject>
	concept ICloser = requires(T type, ConnectionObject &conn_obj) {
		{ T::CloseConnection(conn_obj) } -> std::same_as<void>;
	};

	template <class T, typename ConnectionObject, unsigned int capacity>
	concept IConnectionHandler = requires(T type, ConnectionObject conn_obj, LockFreeQueue<ConnectionObject, capacity> *queue) {
		{ type.Start() } -> std::same_as<void>;
		{ type.handle_connection(conn_obj) } -> std::same_as<void>;
	};
} // namespace request_processing

#endif // WEBSERVER_CONNECTION_HPP