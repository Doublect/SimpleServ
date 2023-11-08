#include <simpleserv/utility/file_manager.hpp>

#include <simpleserv/utility/expected.hpp>
#include <simpleserv/utility/logger.hpp>
#include <simpleserv/debug_flags.hpp>

#include <array>
#include <cstddef>
#include <iostream>
#include <iterator>
#include <filesystem>
#include <format>
#include <fstream>
#include <ranges>
#include <string>
#include <string_view>
#include <unordered_map>

namespace utility {

	const static std::unordered_map<FileType, std::string> file_type_map = {  // NOLINT(cert-err58-cpp)
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

	const static std::unordered_map<std::string, FileType> extension_map = {  // NOLINT(cert-err58-cpp)
			{".html", FileType::TEXT_HTML},
			{".css", FileType::TEXT_CSS},
			{".js", FileType::TEXT_JS},
			{".png", FileType::IMAGE_PNG},
			{".svg", FileType::IMAGE_SVG},
			{".ttf", FileType::FONT_TTF},
			{".woff", FileType::FONT_WOFF},
			{".woff2", FileType::FONT_WOFF2}
	};

	const static std::unordered_map<FileEncoding, std::string> file_encoding_map = {  // NOLINT(cert-err58-cpp)
			{FileEncoding::NONE, ""},
			{FileEncoding::BROTLI, "br"},
			{FileEncoding::DEFLATE, "deflate"},
			{FileEncoding::GZIP, "gzip"}
	};

	//TODO: Support for compressed files extensions
	FileType encode_filetype(const std::string& filename) {
		const std::string extension = std::filesystem::path(filename).extension().string();
		
		if(extension_map.find(extension) != extension_map.end()) {
			return extension_map.at(extension);
		}

		return FileType::TEXT_PLAIN;
	};

	std::string decode_filetype(FileType filetype) {
		if(file_type_map.find(filetype) != file_type_map.end()) {
			return file_type_map.at(filetype);
		}

		return "text/plain";
	};

	std::string decode_file_encoding(FileEncoding file_encoding) {
		if(file_encoding_map.find(file_encoding) != file_encoding_map.end()) {
			return file_encoding_map.at(file_encoding);
		}

		return "";
	};

	template <typename T, typename E, typename F> requires IsFileFetcher<T, E, F>
	utility::expected<T, E> CacheFileFetcher<T, E, F>::get_file(const std::string &full_path) {
		if constexpr (debug::all(ss_debug_file_manager)) {
			utility::Logger::info("Requested file: " + full_path + "\n");
			utility::Logger::info(std::format("File cache size: {}\n", file_cache.size()));
		}

		if(file_cache.find(full_path) == file_cache.end()) {
			auto returned = file_fetcher.get_file(full_path);
			
			if(returned.has_value()) {
				if constexpr (debug::all(ss_debug_file_manager_cache_miss)) {
					utility::Logger::info("File not found in cache, adding it...\n");
				}
				file_cache[full_path] = create_file_cache_entry<T>(full_path, returned.value());
			} else {
				if constexpr (debug::all(ss_debug_file_manager_cache_miss)) {
					utility::Logger::info("File not found in cache, and could not be fetched from disk.\n");
				}
				return utility::unexpected(returned.error());
			}
		}

		const CacheFileFetcherEntry file_cache_entry = file_cache[full_path];
		return from_file_cache_entry<T>(file_cache_entry);
	};

	template <>
	utility::expected<HTTPFileData, FileManagerException> DiskFileFetcher<HTTPFileData, FileManagerException>::get_file(const std::string& full_path) {
		if constexpr (debug::all(ss_debug_file_manager)) {
			utility::Logger::info("Requested file: " + full_path + "\n");
			utility::Logger::info(std::format("File descriptors: {}\n", file_descriptors.size()));
		}

		if(file_descriptors.find(full_path) == file_descriptors.end()) {
			return utility::unexpected(FileManagerException("File not found: " + full_path));
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

	utility::expected<HTTPFileData, FileManagerException> FileManager::get_file(const std::string& full_path) {
		return file_fetcher.get_file(full_path);
	};

	utility::expected<HTTPFileData, FileManagerException> FileManager::get_file(const std::string& path, const std::string& filename) {
		return file_fetcher.get_file(path + filename);
	}

	// TODO: filetype recognition
	constexpr bool is_extension(std::string_view str, std::string_view extension) {
		const size_t len = extension.length();

		if (str.length() < len) {
			return false;
		}
		return str.substr(str.length() - len, len) == extension;
	}

	const auto supported_extensions = std::to_array<std::string>({".html",  ".js",  ".css", ".png", // NOLINT(cert-err58-cpp)
																						".jpg",   ".svg", ".ttf", ".woff",
																						".woff2", ".ico"});

	constexpr bool is_supported_extension(std::string_view str) {
		return std::ranges::any_of(supported_extensions, [str](std::string_view extension) {
			return is_extension(str, extension);
		});
	}

}; // namespace utility