#ifndef WEBSERVER_CONTENT_CODING_HPP
#define WEBSERVER_CONTENT_CODING_HPP

#include <filesystem>
#include <string>

class Brotli {
public:
	Brotli() = default;
	~Brotli() = default;

	void encode(const std::string& path, const std::string& filename);
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

#endif //WEBSERVER_CONTENT_CODING_HPP