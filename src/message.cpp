
#include <iterator>
#include <map>
#include <stdexcept>
#include <string>

HTTPMethod method_string_to_enum(std::string_view str) {
	std::string string_upper;
	std::transform(str.begin(), str.end(), std::back_inserter(string_upper),
								 [](char c) { return toupper(c); });

	for (int i = 0; i < 8; i++) {
		if (string_upper == HTTP_METHOD_STRINGS[i]) {
			return static_cast<HTTPMethod>(i);
		}
	}

	throw std::runtime_error("Invalid HTTP method string: " + std::string(str));
}

HTTPVersion version_string_to_enum(std::string_view str) {
	std::string string_upper;
	std::transform(str.begin(), str.end(), std::back_inserter(string_upper),
								 [](char c) { return toupper(c); });

	if (string_upper == "HTTP/1.0") {
		return HTTP_1_0;
	} else if (string_upper == "HTTP/1.1") {
		return HTTP_1_1;
	} else if (string_upper == "HTTP/2.0") {
		return HTTP_2_0;
	}

	throw std::runtime_error("Invalid HTTP version string: " + string_upper);
}
