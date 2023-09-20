#ifndef WEBSERVER_FILE_MANAGER_HPP
#define WEBSERVER_FILE_MANAGER_HPP

#include <expected>
#include <mutex>
#include <string>
#include <unordered_map>

enum class FileType {
	TEXT_HTML,
	TEXT_CSS,
	TEXT_JS,
	TEXT_PLAIN,
	IMAGE_PNG,
	IMAGE_SVG,
	FONT_TTF,
	FONT_WOFF,
	FONT_WOFF2
};

enum class FileEncoding {
	NONE,
	BROTLI,
	DEFLATE,
	GZIP
};

struct FileEntry {
	std::string path;
};

struct FileDescriptor {
	std::string filename;
	FileType filetype;
	std::unordered_map<FileEncoding, FileEntry> file_paths;
};

struct HTTPFileData {
	FileType filetype;
	FileEncoding file_encoding;
	std::string content;
};

struct FileCacheEntry {
	std::string filename;
	FileType http_content_type;
	FileEncoding http_content_encoding;
	std::string content;
};

std::string decode_filetype(FileType filetype);
std::string decode_file_encoding(FileEncoding file_encoding);

class FileManagerException : public std::exception {
private:
	std::string message;
public:
	FileManagerException(const std::string& message_in) : message(message_in) {}
	const char* what() const noexcept override {
		return message.c_str();
	}
};

class FileManager {
private:
	std::unordered_map<std::string, FileCacheEntry> file_cache;
	std::unordered_map<std::string, FileDescriptor> file_descriptors;
	std::string encoded_file_prefix = ".encoded";
	std::mutex file_access_mutex;

public:
	FileManager() = default;
	~FileManager() = default;

	void construct_file_descriptors(const std::string& path);
	void generate_encoded_files();

	std::expected<HTTPFileData, FileManagerException> get_file(const std::string& full_path);
	std::expected<HTTPFileData, FileManagerException> get_file(const std::string& path, const std::string& filename);	
};

#endif //WEBSERVER_FILE_MANAGER_HPP