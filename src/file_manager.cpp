#include "include/serverlib/file_manager.hpp"

#include "include/serverlib/content_coding.hpp"

#include <iostream>
#include <filesystem>
#include <fstream>

std::unordered_map<FileType, std::string> file_type_map = {
		{FileType::TEXT_HTML, "text/html"},
		{FileType::TEXT_CSS, "text/css"},
		{FileType::TEXT_JS, "text/javascript"},
		{FileType::TEXT_PLAIN, "text/plain"},
		{FileType::IMAGE_PNG, "image/png"},
		{FileType::IMAGE_SVG, "image/svg+xml"},
		{FileType::FONT_TTF, "font/ttf"},
		{FileType::FONT_WOFF, "font/woff"},
		{FileType::FONT_WOFF2, "font/woff2"}
};

std::unordered_map<std::string, FileType> extension_map = {
		{".html", FileType::TEXT_HTML},
		{".css", FileType::TEXT_CSS},
		{".js", FileType::TEXT_JS},
		{".png", FileType::IMAGE_PNG},
		{".svg", FileType::IMAGE_SVG},
		{".ttf", FileType::FONT_TTF},
		{".woff", FileType::FONT_WOFF},
		{".woff2", FileType::FONT_WOFF2}
};

std::unordered_map<FileEncoding, std::string> file_encoding_map = {
		{FileEncoding::NONE, ""},
		{FileEncoding::BROTLI, "br"},
		{FileEncoding::DEFLATE, "deflate"},
		{FileEncoding::GZIP, "gzip"}
};

//TODO: Support for compressed files extensions
FileType encode_filetype(const std::string& filename) {
	std::string extension = std::filesystem::path(filename).extension().string();
	
	if(extension_map.find(extension) != extension_map.end()) {
		return extension_map[extension];
	}

	return FileType::TEXT_PLAIN;
};

std::string decode_filetype(FileType filetype) {
	if(file_type_map.find(filetype) != file_type_map.end()) {
		return file_type_map[filetype];
	}

	return "text/plain";
};

std::string decode_file_encoding(FileEncoding file_encoding) {
	if(file_encoding_map.find(file_encoding) != file_encoding_map.end()) {
		return file_encoding_map[file_encoding];
	}

	return "";
};

template <typename T, typename E, typename F> requires IsFileFetcher<T, E, F>
std::expected<T, E> FileCache<T, E, F>::get_file(const std::string &full_path) {
	std::cout << "Requested file: " << full_path << std::endl;
	std::cout << "File cache size: " << file_cache.size() << std::endl;
	if(file_cache.find(full_path) == file_cache.end()) {
		auto returned = file_fetcher.get_file(full_path);
		
		if(returned.has_value()) {
			std::cout << "File not found in cache, adding it..." << std::endl;
			file_cache[full_path] = create_file_cache_entry<T>(returned.value());
		} else {
			std::cout << "File not found in cache, and could not be fetched from disk." << std::endl;
			return std::unexpected(returned.error());
		}
	}

	FileCacheEntry file_cache_entry = file_cache[full_path];
	return from_file_cache_entry<T>(file_cache_entry);
};

template <>
std::expected<HTTPFileData, FileManagerException> FileDiskFetcher<HTTPFileData, FileManagerException>::get_file(const std::string& full_path) {
	std::cout << "Requested file: " << full_path << std::endl;
	std::cout << "File descriptors: " << file_descriptors.size() << std::endl;
	if(file_descriptors.find(full_path) == file_descriptors.end()) {
		return std::unexpected(FileManagerException("File not found"));
	}

	FileDescriptor file_descriptor = file_descriptors[full_path];
	FileEncoding file_encoding = FileEncoding::NONE;
	std::string actual_path;

	if(file_descriptor.file_paths.find(FileEncoding::BROTLI) != file_descriptor.file_paths.end()) {
		file_encoding = FileEncoding::BROTLI;
		actual_path = file_descriptor.file_paths[FileEncoding::BROTLI].path;
	} else {
		actual_path = file_descriptor.file_paths[FileEncoding::NONE].path;
	}

	std::string content;
	std::ifstream file(actual_path, std::ios::in | std::ios::binary);
	content = std::string((std::istreambuf_iterator<char>(file)),
													std::istreambuf_iterator<char>());

	return HTTPFileData {
		file_descriptor.filetype,
		file_encoding,
		content
	};
};

std::expected<HTTPFileData, FileManagerException> FileManager::get_file(const std::string& full_path) {
	return file_fetcher.get_file(full_path);
};

std::expected<HTTPFileData, FileManagerException> FileManager::get_file(const std::string& path, const std::string& filename) {
	return file_fetcher.get_file(path + filename);
}