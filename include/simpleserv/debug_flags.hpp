#ifndef WEBSERVER_DEBUG_FLAGS_HPP
#define WEBSERVER_DEBUG_FLAGS_HPP

constexpr bool ss_debug = true;


constexpr bool ss_debug_file_manager = true;
constexpr bool ss_debug_file_manager_cache_miss = ss_debug_file_manager || true; // NOLINT(readability-simplify-boolean-expr)

constexpr bool ss_debug_threadpool_worker = true;

namespace debug {
	template<typename... Args>
	constexpr bool all(Args... flags) {
		return (... && flags) && ss_debug;
	};
} // namespace debug

#endif // WEBSERVER_DEBUG_FLAGS_HPP