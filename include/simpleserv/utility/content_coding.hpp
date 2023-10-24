#ifndef WEBSERVER_CONTENT_CODING_HPP
#define WEBSERVER_CONTENT_CODING_HPP

#include <filesystem>
#include <string>

namespace utility {

class Brotli {
public:
	Brotli() = default;
	~Brotli() = default;

	static std::string encode(const std::string& path, const std::string& filename, const std::string& output_prefix =  "");
	//void decompress();
};

class GZip {
public:
	GZip() = default;
	~GZip() = default;

	std::string encode(const std::string& path, const std::string& filename);
	//void decompress();
};

class FileEncoder {
public:
	FileEncoder() = default;
	~FileEncoder() = default;

	void encode_directory(const std::string& path);
	//void decompress();
};

}; // namespace utility

#endif //WEBSERVER_CONTENT_CODING_HPP