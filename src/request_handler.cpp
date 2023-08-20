#include "include/serverlib/request_handler.hpp"

#include <concepts>
#include <string>

using namespace std::string_literals;

/*
* TODO: Server metadata
*/
HTTPResponse options_request(const HTTPRequest &request) {
	HTTPResponse response(HTTPStatusCode::OK, HTTP_1_1);

	if (request.path() == "*") {
		response.SetHeader("Allow", "GET, HEAD, POST, PUT, DELETE, TRACE, OPTIONS");
		return response;
	}

	return response;
};


class IGETRequestHandler {
public:
	virtual GETResult get(std::string key, GETConditionals conditionals) = 0;
};

template <typename K, typename V>
constexpr std::optional<V> find_if_exists(const std::map<K, V> &map, const V &key) {
	auto it = map.find(key);
	if (it == map.end()) {
		return std::nullopt;
	}
	return it->second;
}

template <GETRequestHandler T>
HTTPResponse get_request(const HTTPRequest &request) {
	T handler;

	std::string key = request.path();
	const auto headers = request.headers();
	GETConditionals conditionals;
	conditionals.modified_since = find_if_exists(headers, "If-Modified-Since"s);
	conditionals.unmodified_since = find_if_exists(headers, "If-Unmodified-Since"s);
	conditionals.match = find_if_exists(headers, "If-Match"s);
	conditionals.none_match = find_if_exists(headers, "If-None-Match"s);

	GETResult result = handler.get(key, conditionals);

	HTTPResponse response;
	switch (result.state) {
	case GETResultState::OK:
		response = HTTPResponse(HTTPStatusCode::OK, HTTP_1_1);
		response.SetContent(result.body);
		break;
	case GETResultState::NOT_FOUND:
		response = HTTPResponse(HTTPStatusCode::NOT_FOUND, HTTP_1_1);
		break;
	case GETResultState::ERROR:
		response = HTTPResponse(HTTPStatusCode::INTERNAL_SERVER_ERROR, HTTP_1_1);
		break;
	case GETResultState::MOVED_PERMANENTLY:
		response = HTTPResponse(HTTPStatusCode::MOVED_PERMANENTLY, HTTP_1_1);
		break;
	}

	return response;
}

class IHEADRequestHandler {
public:
	virtual std::map<std::string, std::string> head(std::string key) = 0;
};

template <HEADRequestHandler T>
HTTPResponse head_request(const HTTPRequest &request) {
	T handler;

	std::string key = request.path();

	std::map<std::string, std::string> result = handler.head(key);

	HTTPResponse response(HTTPStatusCode::OK, HTTP_1_1);
	for (auto const &[key, val] : result) {
		response.SetHeader(key, val);
	}

	return response;
}

struct POSTResult {
	POSTResultState state;
	std::string body;
};

class IPOSTRequestHandler {
public:
	virtual POSTResult post(std::string key, std::string value) = 0;
};

/*
* TODO: Varied error handling
*/
template <POSTRequestHandler T>
HTTPResponse post_request(const HTTPRequest &request) {
	T handler;

	std::string key = request.path();
	std::string value = request.body();

	POSTResult result = handler.post(key, value);

	HTTPResponse response;
	switch (result.state) {
	case POSTResultState::OK:
		response = HTTPResponse(HTTPStatusCode::OK, HTTP_1_1);
		response.SetContent(result.body);
		break;
	case POSTResultState::CREATED:
		response = HTTPResponse(HTTPStatusCode::CREATED, HTTP_1_1);
		response.SetContent(result.body);
		break;
	case POSTResultState::NO_CONTENT:
		response = HTTPResponse(HTTPStatusCode::NO_CONTENT, HTTP_1_1);
		break;
	case POSTResultState::SEE_OTHER:
		response = HTTPResponse(HTTPStatusCode::SEE_OTHER, HTTP_1_1);
		break;
	case POSTResultState::ERROR:
		response = HTTPResponse(HTTPStatusCode::INTERNAL_SERVER_ERROR, HTTP_1_1);
		break;
	}

	return response;
}

class IPUTRequestHandler {
public:
	virtual PUTResult put(std::string key, std::string value) = 0;
};

/*
 * TODO: Varied error handling
 */
template <PUTRequestHandler T>
HTTPResponse put_request(const HTTPRequest &request) {
	T handler;

	std::string key = request.path();
	std::string value = request.body();

	PUTResult result = handler.put(key, value);

	HTTPResponse response;
	switch (result) {
	case PUTResult::OK:
		response = HTTPResponse(HTTPStatusCode::OK, HTTP_1_1);
		break;
	case PUTResult::NOT_FOUND:
		response = HTTPResponse(HTTPStatusCode::NOT_FOUND, HTTP_1_1);
		break;
	case PUTResult::ERROR:
		response = HTTPResponse(HTTPStatusCode::INTERNAL_SERVER_ERROR, HTTP_1_1);
		break;
	case PUTResult::MOVED_PERMANENTLY:
		response = HTTPResponse(HTTPStatusCode::MOVED_PERMANENTLY, HTTP_1_1);
		break;
	case PUTResult::UNIMPLEMENTED:
		response = HTTPResponse(HTTPStatusCode::NOT_IMPLEMENTED, HTTP_1_1);
		break;
	}

	return response;
}

class IDELETERequestHandler {
public:
	virtual DELETEResult del(std::string key) = 0;
};

/*
 * TODO: Varied error handling
 */
template <DELETERequestHandler T>
HTTPResponse delete_request(const HTTPRequest &request) {
	T handler;

	std::string key = request.path();

	DELETEResult result = handler.del(key);

	HTTPResponse response;
	switch (result) {
	case DELETEResult::DELETED:
		response = HTTPResponse(HTTPStatusCode::OK, HTTP_1_1);
		break;
	case DELETEResult::DELETE_ACCEPTED:
		response = HTTPResponse(HTTPStatusCode::ACCEPTED, HTTP_1_1);
		break;
	case DELETEResult::NOT_FOUND:
		response = HTTPResponse(HTTPStatusCode::NOT_FOUND, HTTP_1_1);
		break;
	case DELETEResult::ERROR:
		response = HTTPResponse(HTTPStatusCode::INTERNAL_SERVER_ERROR, HTTP_1_1);
		break;
	}

	return response;
}

HTTPResponse trace_request(const HTTPRequest &request) {
	HTTPResponse response(HTTPStatusCode::OK, HTTP_1_1);

	response.SetContent(request.body());

	return response;
}