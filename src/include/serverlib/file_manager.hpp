#ifndef WEBSERVER_FILE_MANAGER_HPP
#define WEBSERVER_FILE_MANAGER_HPP

#include "utility/content_coding.hpp"

#include <expected>
#include <filesystem>
#include <iostream>
#include <mutex>
#include <string>
#include <unordered_map>

enum class FileType : uint8_t {
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

enum class FileEncoding : uint8_t {
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

//TODO: Support for compressed files extensions
FileType encode_filetype(const std::string& filename);
std::string decode_filetype(FileType filetype);
std::string decode_file_encoding(FileEncoding file_encoding);

template <typename T> requires std::is_same_v<T, HTTPFileData>
inline FileCacheEntry create_file_cache_entry(T value) {
	const HTTPFileData& file_data = value;
	
	return FileCacheEntry{decode_filetype(file_data.filetype), file_data.filetype, file_data.file_encoding, file_data.content};
};

template <typename T> requires std::is_same_v<T, HTTPFileData>
inline T from_file_cache_entry(FileCacheEntry value) {
	return HTTPFileData{value.http_content_type, value.http_content_encoding, value.content};
};

class FileManagerException : public std::exception {
private:
	std::string message;
public:
	FileManagerException(const std::string& message_in) : message(message_in) {}
	const char* what() const noexcept final {
		return message.c_str();
	}
};

//FileManagerException::FileManagerException(const std::string& message_in) : message(message_in) {};

class FileManagerNotFoundException : public FileManagerException {
public:
	FileManagerNotFoundException(const std::string& path) 
		: FileManagerException("Could not find the following file: " + std::string(path)) {}
};

template<typename T, typename E>
class FileFetcher {
	typedef T DataT;
	typedef E ErrorT;

	public:
		virtual std::expected<DataT, ErrorT> get_file(const std::string& full_path) = 0;
};

template<typename T, typename E, typename F>
concept IsFileFetcher = std::is_base_of<FileFetcher<T,E>, F>::value;

// TODO(Hunor): Add cache eviction
template<typename T, typename E, typename F> requires IsFileFetcher<T, E, F>
class FileCache: public FileFetcher<T, E> {
	typedef T DataT;
	typedef E ErrorT;
	typedef F FileFetcher;

private:
	FileFetcher file_fetcher{};
	std::unordered_map<std::string, FileCacheEntry> file_cache;
	std::mutex file_access_mutex;
	uint32_t max_cache_size_bytes = 10000000;

public: 
	FileFetcher *get_file_fetcher() {
		return &file_fetcher;
	}
	virtual std::expected<DataT, ErrorT> get_file(const std::string& full_path) override;
};

template<typename T, typename E>
class FileDiskFetcher: public FileFetcher<T, E> {
	typedef T DataT;
	typedef E ErrorT;


	std::string encoded_file_prefix = ".encoded";
	std::unordered_map<std::string, FileDescriptor> file_descriptors;

public: 
	FileDiskFetcher() = default;
	~FileDiskFetcher() = default;

	void construct_file_descriptors(const std::string& path) {
		const std::filesystem::path dir_path(path);
		//std::cout << "Collecting files from directory: " << dir_path << std::endl;
		for (auto const& dir_entry : std::filesystem::directory_iterator{dir_path}) {
			if(dir_entry.is_directory()) {
				construct_file_descriptors(dir_entry.path().string());
			} else {
				std::string filename = dir_entry.path().filename().string();
				FileType filetype = encode_filetype(filename);
		
				file_descriptors[dir_entry.path().string()] = FileDescriptor {
						filename,
						filetype,
						{
								{FileEncoding::NONE, FileEntry{dir_entry.path().string()}}
						}
				};
				//std::cout << "file_descriptors: " << dir_entry.path().string() << std::endl;
			}
		}
	}

	void generate_encoded_files() {
		for(const auto& [path, file_descriptor] : file_descriptors) {
			//std::cout << "Encoding file: " << path << std::endl;
			if(file_descriptor.filetype == FileType::TEXT_HTML ||
					file_descriptor.filetype == FileType::TEXT_CSS ||
					file_descriptor.filetype == FileType::TEXT_JS) {
				utility::Brotli brotli;
				std::filesystem::path dir_path(path);
				std::string file_path = brotli.encode(dir_path.parent_path(), file_descriptor.filename, encoded_file_prefix);

				file_descriptors[path].file_paths[FileEncoding::BROTLI] = FileEntry{path + ".br"};
			}
		}
	}

	virtual std::expected<DataT, ErrorT> get_file(const std::string& full_path) override;
};

class FileManager {
	typedef FileCache<HTTPFileData, FileManagerException, FileDiskFetcher<HTTPFileData, FileManagerException>> Fetcher;
private:
	std::mutex file_access_mutex;
 	Fetcher file_fetcher{};

public:
	FileManager() = default;
	~FileManager() = default;

	std::expected<HTTPFileData, FileManagerException> get_file(const std::string& full_path);
	std::expected<HTTPFileData, FileManagerException> get_file(const std::string& path, const std::string& filename);	

	void construct_file_descriptors(const std::string& path) {
		(file_fetcher.get_file_fetcher())->construct_file_descriptors(path);
	}

	void generate_encoded_files() {
		(file_fetcher.get_file_fetcher())->generate_encoded_files();
	}
};

// TODO: filetype recognition
constexpr bool is_extension(std::string_view str, std::string_view extension) {
	size_t len = extension.length();

	if (str.length() < len)
		return false;
	return str.substr(str.length() - len, len) == extension;
}

static std::string supported_extensions[]{".html",  ".js",  ".css", ".png",
																					".jpg",   ".svg", ".ttf", ".woff",
																					".woff2", ".ico"};

constexpr bool is_supported_extension(std::string_view str) {
	for (int i = 0; i < 9; i++) {
		if (is_extension(str, supported_extensions[i]))
			return true;
	}

	return false;
}

#endif //WEBSERVER_FILE_MANAGER_HPP