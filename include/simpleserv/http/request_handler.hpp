#ifndef REQUEST_HANDLER_HPP
#define REQUEST_HANDLER_HPP

#include <simpleserv/http/message.hpp>

#include <cstdint>
#include <optional>
#include <string>

namespace http {

	HTTPResponse options_request(const HTTPRequest& request);

	// GET request handling

	enum class GETResultState : uint8_t { OK, NOT_FOUND, ERROR, MOVED_PERMANENTLY };

	struct GETResult {
		GETResultState state;
		std::string body;
	};

	using GETCondition = std::optional<std::string>;

	struct GETConditionals {
		GETCondition modified_since;
		GETCondition unmodified_since;
		GETCondition match;
		GETCondition none_match;
	};

	class IGETRequestHandler;

	template <typename T>
	concept GETRequestHandler = std::is_base_of_v<IGETRequestHandler, T>;

	// HEAD request handling

	class IHEADRequestHandler;

	template <typename T>
	concept HEADRequestHandler = std::is_base_of_v<IHEADRequestHandler, T>;

	template <HEADRequestHandler T>
	HTTPResponse head_request(const HTTPRequest& request);

	// POST request handling
	enum class POSTResultState : uint8_t { OK, CREATED, NO_CONTENT, SEE_OTHER, ERROR };

	struct POSTResult {
		POSTResultState state;
		std::string body;
	};

	class IPOSTRequestHandler;

	template <typename T>
	concept POSTRequestHandler = std::is_base_of_v<IPOSTRequestHandler, T>;

	// PUT request handling

	enum class PUTResult : uint8_t { OK, NOT_FOUND, ERROR, MOVED_PERMANENTLY, UNIMPLEMENTED };

	class IPUTRequestHandler;

	template <typename T>
	concept PUTRequestHandler = std::is_base_of_v<IPUTRequestHandler, T>;

	template <PUTRequestHandler T>
	HTTPResponse put_request(const HTTPRequest& request);

	// DELETE request handling

	enum class DELETEResult : uint8_t { DELETED, DELETE_ACCEPTED, NOT_FOUND, ERROR };

	class IDELETERequestHandler;

	template <typename T>
	concept DELETERequestHandler = std::is_base_of_v<IDELETERequestHandler, T>;

	template <DELETERequestHandler T>
	HTTPResponse delete_request(const HTTPResponse& response);

	// TRACE request handling

	HTTPResponse trace_request(const HTTPRequest& request);

} // namespace http

#endif // REQUEST_HANDLER_HPP