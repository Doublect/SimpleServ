#include <simpleserv/http/request_handler.hpp>

#include <simpleserv/http/http.hpp>
#include <simpleserv/http/message.hpp>

#include <map>
#include <optional>
#include <string>

namespace http {

	using namespace std::literals::string_literals;

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
		IGETRequestHandler() = default;

		IGETRequestHandler(const IGETRequestHandler &) = delete;
		IGETRequestHandler &operator=(const IGETRequestHandler &) = delete;
		IGETRequestHandler(IGETRequestHandler &&) = delete;
		IGETRequestHandler &operator=(IGETRequestHandler &&) = delete;

		virtual ~IGETRequestHandler() = default;

		virtual GETResult get(std::string path, GETConditionals conditionals) = 0;

	protected:
	};

	template <typename K, typename V>
	constexpr std::optional<V> find_if_exists(const std::map<K, V> &map, const V &key) {
		auto iter = map.find(key);
		if (iter == map.end()) {
			return std::nullopt;
		}
		return iter->second;
	}

	template <GETRequestHandler T>
	HTTPResponse get_request(const HTTPRequest &request) {
		T handler;

		std::string path = request.path();
		const auto headers = request.headers();
		GETConditionals conditionals;
		conditionals.modified_since = find_if_exists(headers, "If-Modified-Since"s);
		conditionals.unmodified_since = find_if_exists(headers, "If-Unmodified-Since"s);
		conditionals.match = find_if_exists(headers, "If-Match"s);
		conditionals.none_match = find_if_exists(headers, "If-None-Match"s);

		GETResult result = handler.get(path, conditionals);

		HTTPResponse response {HTTPStatusCode::OK, HTTP_1_1};
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
		IHEADRequestHandler() = default;

		IHEADRequestHandler(const IHEADRequestHandler &) = delete;
		IHEADRequestHandler &operator=(const IHEADRequestHandler &) = delete;
		IHEADRequestHandler(IHEADRequestHandler &&) = delete;
		IHEADRequestHandler &operator=(IHEADRequestHandler &&) = delete;

		virtual ~IHEADRequestHandler() = default;

		virtual std::map<std::string, std::string> head(std::string path) = 0;
	protected:
	};

	template <HEADRequestHandler T>
	HTTPResponse head_request(const HTTPRequest &request) {
		T handler;

		const std::string path = request.path();

		std::map<std::string, std::string> result = handler.head(path);

		HTTPResponse response(HTTPStatusCode::OK, HTTP_1_1);
		for (auto const &[key, val] : result) {
			response.SetHeader(key, val);
		}

		return response;
	}

	class IPOSTRequestHandler {
	public:
		IPOSTRequestHandler() = default;

		IPOSTRequestHandler(const IPOSTRequestHandler &) = delete;
		IPOSTRequestHandler &operator=(const IPOSTRequestHandler &) = delete;
		IPOSTRequestHandler(IPOSTRequestHandler &&) = delete;
		IPOSTRequestHandler &operator=(IPOSTRequestHandler &&) = delete;

		virtual ~IPOSTRequestHandler() = default;

		virtual POSTResult post(std::string path, std::string value) = 0;

	protected:
	};

	/*
	* TODO: Varied error handling
	*/
	template <POSTRequestHandler T>
	HTTPResponse post_request(const HTTPRequest &request) {
		T handler;

		const std::string path = request.path();
		const std::string value = request.body();

		POSTResult result = handler.post(path, value);

		HTTPResponse response {HTTPStatusCode::OK, HTTP_1_1};
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
		IPUTRequestHandler() = default;

		IPUTRequestHandler(const IPUTRequestHandler &) = delete;
		IPUTRequestHandler &operator=(const IPUTRequestHandler &) = delete;
		IPUTRequestHandler(IPUTRequestHandler &&) = delete;
		IPUTRequestHandler &operator=(IPUTRequestHandler &&) = delete;

		virtual ~IPUTRequestHandler() = default;

		virtual PUTResult put(std::string path, std::string value) = 0;

	protected:
	};

	/*
	* TODO: Varied error handling
	*/
	template <PUTRequestHandler T>
	HTTPResponse put_request(const HTTPRequest &request) {
		T handler;

		const std::string path = request.path();
		const std::string value = request.body();

		PUTResult result = handler.put(path, value);

		HTTPResponse response  {HTTPStatusCode::OK, HTTP_1_1};
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
		IDELETERequestHandler() = default;

		IDELETERequestHandler(const IDELETERequestHandler &) = delete;
		IDELETERequestHandler &operator=(const IDELETERequestHandler &) = delete;
		IDELETERequestHandler(IDELETERequestHandler &&) = delete;
		IDELETERequestHandler &operator=(IDELETERequestHandler &&) = delete;

		virtual ~IDELETERequestHandler() = default;
	
		virtual DELETEResult del(std::string path) = 0;

	protected:
	};

	/*
	* TODO: Varied error handling
	*/
	template <DELETERequestHandler T>
	HTTPResponse delete_request(const HTTPRequest &request) {
		T handler;

		const std::string path = request.path();

		DELETEResult result = handler.del(path);

		HTTPResponse response  {HTTPStatusCode::OK, HTTP_1_1};
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

} // namespace http