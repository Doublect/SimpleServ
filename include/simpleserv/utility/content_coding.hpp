#ifndef WEBSERVER_CONTENT_CODING_HPP
#define WEBSERVER_CONTENT_CODING_HPP

#include <filesystem>
#include <string>

namespace utility {

class Brotli {
public:
	static std::filesystem::path encode(const std::filesystem::path &path);
	//void decompress();
};

class GZip {
public:
	std::string encode(const std::string& path, const std::string& filename);
	//void decompress();
};

class FileEncoder {
public:
	static void encode_directory(const std::string& path);
	//void decompress();
};

}; // namespace utility

#endif //WEBSERVER_CONTENT_CODING_HPP