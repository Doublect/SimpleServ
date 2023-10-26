#ifndef SIMPLESERV_STRING_PARSER
#define SIMPLESERV_STRING_PARSER

#include <simpleserv/utility/expected.hpp>

#include <format>
#include <functional>
#include <source_location>
#include <string>
#include <string_view>
#include <tuple>

namespace utility {

		class parser_error final : public std::exception {
	public:
		parser_error(const std::source_location location = std::source_location::current()) : location(location) {}
		parser_error(std::string message, const std::source_location location = std::source_location::current()) : location(location) {}
		~parser_error() final {}

		const char *what() const noexcept override {
			if(message.empty()) {
				return "An error has occured during parsing.";
			}

			return std::format("Parser error in {}, line {}, function {}\n {}", location.file_name(), location.line(), location.function_name(), message).c_str();
		}

	private:
		std::source_location location;
		std::string message;

	};

	class StringParser {
	public:
		StringParser(std::string str) : str(str) {}

		inline void move(size_t steps) {
			view.remove_prefix(steps);
		}

		inline utility::expected<void, parser_error> expect(char char1) {
			if(!view.starts_with(char1)) {
				return utility::unexpected(parser_error(std::format("Expected character: {} ({}), got {} ({})", (char1), static_cast<int>(char1), str, static_cast<int>(view[0]))));
			}
			view.remove_prefix(1);
		}

		inline utility::expected<void, parser_error> expect(const std::string_view chars) {
			if(!view.starts_with(chars)) {
				// TODO: string escaping
				return utility::unexpected(parser_error(std::format("Expected characters: {}, got {}", chars, str)));
			}

			view.remove_prefix(chars.length());
		}

		inline utility::expected<void, parser_error> expect_two(char char1, char char2) {
			if(!view.starts_with(char1) && !view.starts_with(char2)) {
				return utility::unexpected(parser_error(std::format("Expected character: {} ({}) or {} ({}), got {} ({})", (char1), static_cast<int>(char1), char2, static_cast<int>(char2), str, static_cast<int>(view[0]))));
			}
			view.remove_prefix(1);
		}

		inline void consume_all(char char1) {
			while(view.starts_with(char1)) {
				view.remove_prefix(1);
			}
		}

		inline void consume_all_two(char char1, char char2) {
			while(view.starts_with(char1) || view.starts_with(char2)) {
				view.remove_prefix(1);
			}
		}

		inline char head() {
			return view.at(0);
		}

		// template<typename T>
		// inline utility::expected<T, parser_error> process_function(std::function<utility::expected<T, parser_error>(std::string_view&)> function) {
		// 	function(view);
		// }

		template<typename T>
		inline T process_function(std::function<T(std::string_view&)> function) {
			function(view);
		}

		template<typename T>
		inline T process_function_own(std::function<T(std::string_view)> function) {
			function(std::move(view));
		}

		constexpr std::string_view next_token(const char delim = ' ') {
			const size_t end = view.find_first_of(delim);
			const std::string_view token = view.substr(0, end);
			view.remove_prefix(end);

			return token;
		}

		constexpr static std::tuple<std::string_view, std::string_view> next_token(std::string_view sv, const char delim = ' ') {
			const size_t end = sv.find_first_of(delim);
			const std::string_view token = sv.substr(0, end);
			sv.remove_prefix(end);

			return std::make_tuple(token, sv);
		}

	private:
		const std::string str;
		std::string_view view {str};
		const size_t length = str.length();
		std::string::const_iterator pointer = str.begin();
	};
}

#endif // SIMPLESERV_STRING_PARSER