#include <simpleserv/utility/url.hpp>

#include <simpleserv/utility/expected.hpp>

#include <cstddef>
#include <map>
#include <string>

namespace utility {

	inline std::string path(std::string str) {
		const size_t query_index = str.find('?');
		if (query_index != std::string::npos) {
			return str.substr(0, query_index);
		}

		return str;
	}

	inline std::map<std::string, std::string> searchpart(const std::string& str, size_t start) {
		std::map<std::string, std::string> query;

		size_t query_index = start;

		while (true) {
			const size_t eq_pos = str.find('=', query_index);
			size_t amp_pos = str.find('&', query_index);

			amp_pos = amp_pos == std::string::npos ? str.length() : amp_pos;

			if (eq_pos == std::string::npos) {
				break;
			}

			const std::string key = str.substr(query_index, eq_pos - query_index);
			const std::string value = str.substr(eq_pos + 1, amp_pos - eq_pos - 1);

			query[key] = value;

			query_index = amp_pos + 1;
		}

		return query;
	}

	auto from_string(const std::string& str) -> utility::expected<URL, std::string> {
		const std::string path_str = path(str);
		const std::map<std::string, std::string> query =
				searchpart(str, path_str.length() + 1);

		return URL(path_str, query);
	}

	inline std::string percent_decode(std::string str) {
		std::string decoded;

		for (size_t i = 0; i < str.length(); i++) {
			if (str[i] == '%') {
				const std::string hex = str.substr(i + 1, 2);
				const int char_code = std::stoi(hex, nullptr, 16);
				decoded += static_cast<char>(char_code);
				i += 2;
			} else {
				decoded += str[i];
			}
		}

		return decoded;
	}

	inline std::string percent_encode(const std::string& str) {
		std::string encoded;

		for (auto current_char : str) {
			switch (current_char) {
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
					encoded += "%" + std::to_string(static_cast<int>(current_char));
					break;

				default:
					encoded += current_char;
					break;
			}
		}

		return encoded;
	}

} // namespace utility