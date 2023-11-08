#ifndef SIMPLESERV_STRING_PARSER
#define SIMPLESERV_STRING_PARSER

#include <simpleserv/utility/expected.hpp>

#include <format>
#include <functional>
#include <optional>
#include <source_location>
#include <string>
#include <string_view>
#include <tuple>

namespace utility {

	class parser_error final : public std::exception {
	public:
		explicit parser_error(const std::source_location location = std::source_location::current()) : location_(location) {}
		explicit parser_error(std::string message, const std::source_location location = std::source_location::current()) : location_(location) {
			msg_ = std::format("Parser error in {}, line {}, function {}\n {}", location_.file_name(), location_.line(), location_.function_name(), message);
		}

		parser_error(const parser_error&) = default;
		parser_error& operator=(const parser_error&) = default;
		parser_error(parser_error&&) noexcept = default;
		parser_error& operator=(parser_error&&) noexcept = default;

		~parser_error() final = default;

		[[nodiscard]] const char *what() const noexcept override {
			return msg_.c_str();
		}

	private:
		std::source_location location_;
		std::string msg_ = "An error has occured during parsing.";

	};

	class StringParser {
	public:
		explicit StringParser(const std::string& str_in) : str(str_in) {}
		explicit StringParser(std::string&& str_in) : str(std::move(str_in)) {}

		inline void move(size_t steps) {
			view.remove_prefix(steps);
		}

		inline std::optional<parser_error> expect(char char1) {
			if(!view.starts_with(char1)) {
				return parser_error(std::format("Expected character: {} ({}), got {} ({})", (char1), static_cast<int>(char1), view[0], static_cast<int>(view[0])));
			}

			view.remove_prefix(1);
			return std::nullopt;
		}

		inline std::optional<parser_error> expect(const std::string_view& chars, const std::source_location location = std::source_location::current()) {
			if(!view.starts_with(chars)) {
				// TODO: string escaping
				return parser_error(std::format("Expected characters: {}, got {}", chars, view), location);
			}

			view.remove_prefix(chars.length());
			return std::nullopt;
		}

		inline std::optional<parser_error> expect_two(char char1, char char2) {
			if(!view.starts_with(char1) && !view.starts_with(char2)) {
				return parser_error(std::format("Expected character: {} ({}) or {} ({}), got {} ({})", (char1), static_cast<int>(char1), char2, static_cast<int>(char2), str, static_cast<int>(view[0])));
			}
			view.remove_prefix(1);
			return std::nullopt;
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
			return function(view);
		}

		template<typename T>
		inline T process_function_own(std::function<T(std::string_view)> function) {
			return function(view);
		}

		constexpr std::string_view next_token(const char delim = ' ') {
			const size_t end = view.find_first_of(delim);
			const std::string_view token = view.substr(0, end);
			view.remove_prefix(end);

			return token;
		}

		constexpr static std::tuple<std::string_view, std::string_view> next_token(std::string_view view, const char delim = ' ') {
			const size_t end = view.find_first_of(delim);
			const std::string_view token = view.substr(0, end);
			view.remove_prefix(end);

			return std::make_tuple(token, view);
		}

	private:
		std::string str;
		std::string_view view {str};
	};
}

#endif // SIMPLESERV_STRING_PARSER