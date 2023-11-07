#ifndef WEBSERVER_DEBUG_FLAGS_HPP
#define WEBSERVER_DEBUG_FLAGS_HPP

constexpr bool _debug = true;


constexpr bool _debug_file_manager = true;
constexpr bool _debug_file_manager_cache_miss = _debug_file_manager || true;

constexpr bool _debug_threadpool_worker = true;

namespace debug {
	template<typename... Args>
	constexpr bool all(Args... flags) {
		return (... && flags) && _debug;
	};
} // namespace debug

#endif // WEBSERVER_DEBUG_FLAGS_HPP