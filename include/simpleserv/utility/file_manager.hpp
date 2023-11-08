#ifndef WEBSERVER_FILE_MANAGER_HPP
#define WEBSERVER_FILE_MANAGER_HPP

#include <simpleserv/http/parser.hpp>
#include <simpleserv/utility/content_coding.hpp>
#include <simpleserv/utility/expected.hpp>

#include <filesystem>
#include <iostream>
#include <mutex>
#include <ranges>
#include <stack>
#include <string>
#include <unordered_map>

namespace utility {

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

	/// The FileDescriptor describes a file and maintains the paths to all different representations
	/// of it present.
	struct FileDescriptor {
		std::string filename;
		FileType filetype = FileType::TEXT_PLAIN;
		std::unordered_map<FileEncoding, FileEntry> file_paths;
	};

	struct HTTPFileData {
		FileType filetype = FileType::TEXT_PLAIN;
		FileEncoding file_encoding = FileEncoding::NONE;
		std::string content;
	};

	template<typename T>
	struct CacheFileFetcherEntry {
		std::string filename;
		T value;
	};

	//TODO: Support for compressed files extensions
	FileType encode_filetype(const std::string& filename);
	std::string decode_filetype(FileType filetype);
	std::string decode_file_encoding(FileEncoding file_encoding);

	template <typename T>
	inline CacheFileFetcherEntry<T> create_file_cache_entry(std::string filename, T data) {	
		return CacheFileFetcherEntry<T>{filename, data};
	};

	template <typename T>
	inline T from_file_cache_entry(CacheFileFetcherEntry<T> cache_entry) {
		return cache_entry.value;
	};

	class FileManagerException : public std::exception {
	private:
		std::string message;
	public:
		explicit FileManagerException(const std::string& message_in) : message(message_in) {}
		explicit FileManagerException(std::string&& message_in) : message(std::move(message_in)) {}
		[[nodiscard]] const char* what() const noexcept final {
			return message.c_str();
		}
	};

	class FileManagerNotFoundException : public FileManagerException {
	public:
		explicit FileManagerNotFoundException(const std::string& path) 
			: FileManagerException("Could not find the following file: " + path) {}
		explicit FileManagerNotFoundException(std::string&& path) 
			: FileManagerException("Could not find the following file: " + std::move(path)) {}
	};

	template<typename T, typename E>
	class FileFetcher {
		using DataT = T;
		using ErrorT = E;

		public:
			FileFetcher() = default;

			FileFetcher(const FileFetcher&) = delete;
			FileFetcher(FileFetcher&&) = delete;
			FileFetcher& operator=(const FileFetcher&) = delete;
			FileFetcher& operator=(FileFetcher&&) = delete;

			virtual ~FileFetcher() = default;

			virtual utility::expected<DataT, ErrorT> get_file(const std::string& full_path) = 0;
	};

	template<typename T, typename E, typename F>
	concept IsFileFetcher = std::is_base_of_v<FileFetcher<T,E>, F>;

	constexpr size_t max_cache_size_bytes = 10000000;

	// TODO(Hunor): Add cache eviction
	template<typename T, typename E, typename F> requires IsFileFetcher<T, E, F>
	class CacheFileFetcher: public FileFetcher<T, E> {
		using DataT = T;
		using ErrorT = E;
		using FileFetcher = F;

	public:	
		CacheFileFetcher() = default;

		CacheFileFetcher(const CacheFileFetcher&) = delete;
		CacheFileFetcher& operator=(const CacheFileFetcher&) = delete;
		CacheFileFetcher(CacheFileFetcher&&) = delete;
		CacheFileFetcher& operator=(CacheFileFetcher&&) = delete;

		~CacheFileFetcher() override = default;

		FileFetcher *get_file_fetcher() {
			return &file_fetcher;
		}

		utility::expected<DataT, ErrorT> get_file(const std::string& full_path) override;

	private:
		FileFetcher file_fetcher{};
		std::unordered_map<std::string, CacheFileFetcherEntry<DataT>> file_cache;
		std::mutex file_access_mutex;
		uint32_t max_cache_size_bytes_ = max_cache_size_bytes;

	};

	template<typename T, typename E>
	class DiskFileFetcher: public FileFetcher<T, E> {
		using DataT = T;
		using ErrorT = E;


		// std::string encoded_file_prefix = ".encoded";
		std::unordered_map<std::string, FileDescriptor> file_descriptors;

	public: 
		DiskFileFetcher() = default;

		DiskFileFetcher(const DiskFileFetcher&) = delete;
		DiskFileFetcher& operator=(const DiskFileFetcher&) = delete;
		DiskFileFetcher(DiskFileFetcher&&) = delete;
		DiskFileFetcher& operator=(DiskFileFetcher&&) = delete;

		~DiskFileFetcher() override = default;

		void construct_file_descriptors(const std::string& path) {
			std::stack<std::filesystem::path> directories {{path}};

			while(!directories.empty()) {
				const std::filesystem::path dir_path(directories.top());
				directories.pop();
				// std::cout << "Collecting files from directory: " << dir_path << "\n";
				for (auto const& dir_entry : std::filesystem::directory_iterator{dir_path}) {
					if(dir_entry.is_directory()) {
						directories.push(dir_entry.path());
					} else {
						const std::string filename = dir_entry.path().filename().string();
						const FileType filetype = encode_filetype(filename);
				
						file_descriptors[dir_entry.path().string()] = FileDescriptor {
								filename,
								filetype,
								{
										{FileEncoding::NONE, FileEntry{dir_entry.path().string()}}
								}
						};
						// std::cout << "file_descriptors: " << dir_entry.path().string() << "\n";
					}
				}
			}
		}

		void generate_encoded_files() {
			for(const auto& [path, file_descriptor] : file_descriptors) {
				std::cout << "Encoding file: " << path << "\n";
				if(file_descriptor.filetype == FileType::TEXT_HTML ||
						file_descriptor.filetype == FileType::TEXT_CSS ||
						file_descriptor.filetype == FileType::TEXT_JS) {
					const utility::Brotli brotli;
					const std::filesystem::path dir_path(path);
					const std::filesystem::path file_path(dir_path.parent_path() /= file_descriptor.filename);
					const std::filesystem::path output_path = brotli.encode(file_path);
					
					if(output_path != "") {
						std::cout << "Encoded to: " << path << "\n";
						file_descriptors[path].file_paths[FileEncoding::BROTLI] = FileEntry{output_path};
					}
				}
			}
		}

		utility::expected<DataT, ErrorT> get_file(const std::string& full_path) override;
	};

	class FileManager {
		using Fetcher = CacheFileFetcher<HTTPFileData, FileManagerException, DiskFileFetcher<HTTPFileData, FileManagerException>>;
	private:
		std::mutex file_access_mutex;
		Fetcher file_fetcher{};

	public:
		utility::expected<HTTPFileData, FileManagerException> get_file(const std::string& full_path);
		utility::expected<HTTPFileData, FileManagerException> get_file(const std::string& path, const std::string& filename);	

		utility::expected<std::vector<http::Header>, FileManagerException> get_file_headers();

		void construct_file_descriptors(const std::string& path) {
			(file_fetcher.get_file_fetcher())->construct_file_descriptors(path);
		}

		void generate_encoded_files() {
			(file_fetcher.get_file_fetcher())->generate_encoded_files();
		}
	};

	constexpr bool is_extension(std::string_view str, std::string_view extension);
	constexpr bool is_supported_extension(std::string_view str);

}; // namespace utility

#endif //WEBSERVER_FILE_MANAGER_HPP