#ifndef SIMPLESERV_LOGGER_HPP
#define SIMPLESERV_LOGGER_HPP

#include <iostream>
#include <string>
#include <boost/log/trivial.hpp>

namespace utility {

	// A class wrapping the Boost logging functions
	class Logger {
	public:
		static inline void trace(const std::string& msg) noexcept {
			BOOST_LOG_TRIVIAL(trace) << msg;
		};

		static inline void debug(const std::string& msg) noexcept {
			BOOST_LOG_TRIVIAL(debug) << msg;
		};

		static inline void info(const std::string& msg) noexcept {
			BOOST_LOG_TRIVIAL(info) << msg;
		};

		static inline void warning(const std::string& msg) noexcept {
			BOOST_LOG_TRIVIAL(warning) << msg;
		};

		static inline void error(const std::string& msg) noexcept {
			BOOST_LOG_TRIVIAL(error) << msg;
		};

		static inline void fatal(const std::string& msg) noexcept {
			BOOST_LOG_TRIVIAL(fatal) << msg;
		};
	};

} // namespace utility

#endif // SIMPLESERV_LOGGER_HPP