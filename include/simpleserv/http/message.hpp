#ifndef WEBSERVER_MESSAGE_HPP
#define WEBSERVER_MESSAGE_HPP

#include <simpleserv/http/http.hpp>
#include <simpleserv/utility/expected.hpp>
#include <simpleserv/utility/string_parser.hpp>

#include <algorithm>
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

	const inline utility::expected<HTTPMethod, utility::parser_error> method_string_to_enum(utility::StringParser& sp) {
		switch (sp.head()) {
			case 'G':
				sp.move(1);
				return transform_fn(HTTPMethod::GET)(sp.expect("ET"));

			case 'P':
				sp.move(1);
				if(sp.head() == 'O') {
					sp.move(1);
					return transform_fn(HTTPMethod::POST)(sp.expect("ST"));
				} else {
					return transform_fn(HTTPMethod::PUT)(sp.expect("UT"));
				}

			case 'D':
				sp.move(1);
				return transform_fn(HTTPMethod::DELETE)(sp.expect("ELETE"));

			case 'H':
				sp.move(1);
				return transform_fn(HTTPMethod::HEAD)(sp.expect("EAD"));

			case 'O':
				sp.move(1);
				return transform_fn(HTTPMethod::OPTIONS)(sp.expect("PTIONS"));

			case 'T':
				sp.move(1);
				return transform_fn(HTTPMethod::TRACE)(sp.expect("RACE"));

			case 'C':
				sp.move(1);
				return transform_fn(HTTPMethod::CONNECT)(sp.expect("ONNECT"));

			default:
				break;
		}

		return utility::unexpected(utility::parser_error("Invalid HTTP method string: " + std::string(sp.next_token())));
	}

	enum HTTPVersion : uint8_t { HTTP_1_0, HTTP_1_1, HTTP_2_0, HTTP_INVALID };

	constexpr const char *HTTP_VERSION_STRINGS[] = {"HTTP/1.0", "HTTP/1.1",
																							"HTTP/2.0"};


	#define EXPECT_TH(str, char)\
		if(*str != char || *str == '\0') {\
			throw std::runtime_error("Invalid HTTP version string");\
		}\
		str++;

	constexpr HTTPVersion version_string_to_enum(utility::StringParser& sp) {
		sp.expect("HTTP/");
		const char head = sp.head();
		HTTPVersion version = HTTP_INVALID;

		if(head == '2') {
			sp.move(1);
			sp.expect(".0");
			version = HTTP_2_0;
		} else {
			sp.move(1);
			sp.expect("1.");
			const char head1 = sp.head();
			if(head1 == '0') {
				sp.move(1);
				version = HTTP_1_0;
			} else {
				sp.move(1);
				sp.expect("1");
				version = HTTP_1_1;
			}
		}

		return version;
	}

	class HTTPMessage {
	protected:
		HTTPVersion version_;
		std::map<std::string, std::string> headers_;
		std::string body_;

	public:
		void SetHeader(const std::string &key, std::string value) {
		headers_[key] = std::move(value);
	};

		void SetContent(std::string content) { body_ = content; }

		HTTPVersion version() const { return version_; }
		std::map<std::string, std::string> headers() const { return headers_; }
		std::string body() const { return body_; }
	};

	class HTTPRequest : public HTTPMessage {
	private:
		HTTPMethod method_;
		std::string path_;

	public:
		HTTPRequest(HTTPMethod method, std::string path, HTTPVersion version) : method_(method), path_(path) {
			this->version_ = version;
		}

		inline HTTPMethod method() const { return method_; }
		inline std::string path() const { return path_; }

		friend std::ostream &operator<<(std::ostream &os, const HTTPRequest &request) {
			os << HTTP_METHOD_STRINGS[request.method()] << " " << request.path() << " "
				<< HTTP_VERSION_STRINGS[request.version()] << "\r\n";
			for (const auto &header : request.headers()) {
				os << header.first << ": " << header.second << "\r\n";
			}
			os << "\r\n";
			os << request.body();

			return os;
		}

		std::string to_string() const {
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

		HTTPStatusCode status_code() const { return status_code_; }

		friend std::ostream &operator<<(std::ostream &os, const HTTPResponse &response) {
			os << HTTP_VERSION_STRINGS[response.version()] << " "
					<< status_code_to_string(response.status_code()) << "\r\n";
			for (const auto &header : response.headers()) {
				os << header.first << ": " << header.second << "\r\n";
			}
			os << "\r\n";
			os << response.body();

			return os;
		}

		std::string to_string() const {
			std::ostringstream oss;

			operator<<(oss, *this);

			return oss.str();
		}
	};

} // namespace http

#endif // WEBSERVER_MESSAGE_HPP