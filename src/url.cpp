#include "include/serverlib/url.hpp"

inline std::string path(std::string str) {
	size_t query_index = str.find("?");
	if (query_index != std::string::npos) {
		return str.substr(0, query_index);
	}

	return str;
}

inline std::map<std::string, std::string> searchpart(std::string str, size_t start) {
	std::map<std::string, std::string> query;

	size_t query_index = start;

	while (true) {
		size_t eq_pos = str.find("=", query_index);
		size_t amp_pos = str.find("&", query_index);

		amp_pos = amp_pos == std::string::npos ? str.length() : amp_pos;

		if (eq_pos == std::string::npos) {
			break;
		}

		std::string key = str.substr(query_index, eq_pos - query_index);
		std::string value = str.substr(eq_pos + 1, amp_pos - eq_pos - 1);

		query[key] = value;

		query_index = amp_pos + 1;
	}

	return query;
}

auto from_string(std::string str) -> std::expected<URL, std::string> {
	std::string path_str = path(str);
	std::map<std::string, std::string> query =
			searchpart(str, path_str.length() + 1);

	return URL(path_str, query);
}

inline std::string percent_decode(std::string str) {
	std::string decoded = "";

	for (size_t i = 0; i < str.length(); i++) {
		if (str[i] == '%') {
			std::string hex = str.substr(i + 1, 2);
			int char_code = std::stoi(hex, nullptr, 16);
			decoded += static_cast<char>(char_code);
			i += 2;
		} else {
			decoded += str[i];
		}
	}

	return decoded;
}

inline std::string percent_encode(std::string str) {
	std::string encoded = "";

	for (size_t i = 0; i < str.length(); i++) {
		switch (str[i]) {
		case (' '):
		case ('!'):
		case ('"'):
		case ('#'):
		case ('$'):
		case ('%'):
		case ('&'):
		case ('\''):
		case ('('):
		case (')'):
		case ('*'):
		case ('+'):
		case (','):
		case ('/'):
		case (':'):
		case (';'):
		case ('='):
		case ('?'):
		case ('@'):
		case ('['):
		case (']'):
			// case('的'):
			encoded += "%" + std::to_string(static_cast<int>(str[i]));
			break;

		default:
			encoded += str[i];
			break;
		}
	}

	return encoded;
}