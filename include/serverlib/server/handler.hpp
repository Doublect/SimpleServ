#ifndef WEBSERVER_SERVER_HANDLER_HPP
#define WEBSERVER_SERVER_HANDLER_HPP

#include <concepts>
#include <string>
#include <type_traits>
#include <optional>

namespace server {

	template <typename T>
	concept IHandler = requires(T handler, std::string str)
	{
		{ handler.handle(str) } -> std::same_as<std::optional<std::string>>;
	};

};

#endif // WEBSERVER_SERVER_HANDLER_HPP