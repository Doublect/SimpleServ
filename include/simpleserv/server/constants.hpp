#ifndef SIMPLESERV_SERVER_CONSTANTS_HPP
#define SIMPLESERV_SERVER_CONSTANTS_HPP

#include <cstdint>
#include <cstddef>

namespace server {

	constexpr size_t MAXBUFLEN = 4096;
	constexpr uint32_t HTTP_PORT = 80;
	constexpr uint32_t HTTPS_PORT = 443;

} // namespace server

#endif //SIMPLESERV_SERVER_CONSTANTS_HPP