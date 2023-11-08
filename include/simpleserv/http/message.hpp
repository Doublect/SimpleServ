#ifndef WEBSERVER_MESSAGE_HPP
#define WEBSERVER_MESSAGE_HPP

#include <simpleserv/http/http.hpp>
#include <simpleserv/utility/expected.hpp>
#include <simpleserv/utility/string_parser.hpp>

#include <algorithm>
#include <array>
#include <cstring>
#include <cstdint>
#include <functional>
#include <map>
#include <sstream>
#include <stdexcept>
#include <string>

namespace http {

	inline std::function<utility::expected<HTTPMethod, utility::parser_error>(std::optional<utility::parser_error>)> transform_fn(HTTPMethod method) {
		return [method](std::optional<utility::parser_error> res) -> utility::expected<HTTPMethod, utility::parser_error> {
			if(res) {
				return utility::unexpected(res.value());
			} else {
				return method;
			}
		};
	}

	inline utility::expected<HTTPMethod, utility::parser_error> method_string_to_enum(utility::StringParser& parser) {
		switch (parser.head()) {
			case 'G':
				parser.move(1);
				return transform_fn(HTTPMethod::GET)(parser.expect("ET"));

			case 'P':
				parser.move(1);
				if(parser.head() == 'O') {
					parser.move(1);
					return transform_fn(HTTPMethod::POST)(parser.expect("ST"));
				} else {
					return transform_fn(HTTPMethod::PUT)(parser.expect("UT"));
				}

			case 'D':
				parser.move(1);
				return transform_fn(HTTPMethod::DELETE)(parser.expect("ELETE"));

			case 'H':
				parser.move(1);
				return transform_fn(HTTPMethod::HEAD)(parser.expect("EAD"));

			case 'O':
				parser.move(1);
				return transform_fn(HTTPMethod::OPTIONS)(parser.expect("PTIONS"));

			case 'T':
				parser.move(1);
				return transform_fn(HTTPMethod::TRACE)(parser.expect("RACE"));

			case 'C':
				parser.move(1);
				return transform_fn(HTTPMethod::CONNECT)(parser.expect("ONNECT"));

			default:
				break;
		}

		return utility::unexpected(utility::parser_error("Invalid HTTP method string: " + std::string(parser.next_token())));
	}

	enum HTTPVersion : uint8_t { HTTP_1_0, HTTP_1_1, HTTP_2_0, HTTP_INVALID };

	constexpr const std::array<const char *, 3> HTTP_VERSION_STRINGS = {"HTTP/1.0", "HTTP/1.1", "HTTP/2.0"};


	constexpr HTTPVersion version_string_to_enum(utility::StringParser& parser) {
		parser.expect("HTTP/");
		const char head = parser.head();
		HTTPVersion version = HTTP_INVALID;

		if(head == '2') {
			parser.move(1);
			parser.expect(".0");
			version = HTTP_2_0;
		} else {
			parser.move(1);
			parser.expect("1.");
			const char head1 = parser.head();
			if(head1 == '0') {
				parser.move(1);
				version = HTTP_1_0;
			} else {
				parser.move(1);
				parser.expect("1");
				version = HTTP_1_1;
			}
		}

		return version;
	}

	class HTTPMessage {
	protected:
		HTTPVersion version_{HTTP_1_0};
		std::map<std::string, std::string> headers_;
		std::string body_;

	public:
		void SetHeader(const std::string &key, std::string value) {
		headers_[key] = std::move(value);
	};

		void SetContent(std::string content) { body_ = std::move(content); }

		[[nodiscard]] HTTPVersion version() const { return version_; }
		[[nodiscard]] std::map<std::string, std::string> headers() const { return headers_; }
		[[nodiscard]] std::string body() const { return body_; }
	};

	class HTTPRequest : public HTTPMessage {
	private:
		HTTPMethod method_;
		std::string path_;

	public:
		HTTPRequest(HTTPMethod method, std::string path, HTTPVersion version) : method_(method), path_(std::move(path)) {
			this->version_ = version;
		}

		[[nodiscard]] inline HTTPMethod method() const { return method_; }
		[[nodiscard]] inline std::string path() const { return path_; }

		friend std::ostream &operator<<(std::ostream &ostream, const HTTPRequest &request) {
			ostream << HTTP_METHOD_STRINGS[request.method()] << " " << request.path() << " " 	// NOLINT(cppcoreguidelines-pro-bounds-constant-array-index)
				<< HTTP_VERSION_STRINGS[request.version()] << "\r\n";					 									// NOLINT(cppcoreguidelines-pro-bounds-constant-array-index)
			for (const auto &header : request.headers()) {
				ostream << header.first << ": " << header.second << "\r\n";
			}
			ostream << "\r\n";
			ostream << request.body();

			return ostream;
		}

		[[nodiscard]] std::string to_string() const {
			std::ostringstream oss;

			oss << this;

			return oss.str();
		}
	};

	class HTTPResponse : public HTTPMessage {
	private:
		HTTPStatusCode status_code_;

	public:
		HTTPResponse(HTTPStatusCode status_code, HTTPVersion version) : status_code_(status_code) {
			this->version_ = version;
			this->body_ = "";
		}

		[[nodiscard]] HTTPStatusCode status_code() const { return status_code_; }

		friend std::ostream &operator<<(std::ostream &ostream, const HTTPResponse &response) {
			ostream << HTTP_VERSION_STRINGS[response.version()] << " " // NOLINT(cppcoreguidelines-pro-bounds-constant-array-index)
					<< status_code_to_string(response.status_code()) << "\r\n";
			for (const auto &header : response.headers()) {
				ostream << header.first << ": " << header.second << "\r\n";
			}
			ostream << "\r\n";
			ostream << response.body();

			return ostream;
		}

		[[nodiscard]] std::string to_string() const {
			std::ostringstream oss;

			operator<<(oss, *this);

			return oss.str();
		}
	};

} // namespace http

#endif // WEBSERVER_MESSAGE_HPP
