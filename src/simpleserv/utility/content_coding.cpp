#include <simpleserv/utility/content_coding.hpp>
#include <simpleserv/utility/logger.hpp>

#include <brotli/encode.h>
#include <brotli/types.h>

#include <algorithm>
#include <cstdint>
#include <cstddef>
#include <iostream>
#include <iterator>
#include <filesystem>
#include <fstream>
#include <stack>
#include <string>
#include <vector>

namespace utility {

	bool write_to_file(const std::filesystem::path &output_path, const std::vector<uint8_t>& output_data) {
		// Write to file
		std::ofstream output_file(output_path, std::ios::out | std::ios::binary);
		if(!output_file.good()) {
			utility::Logger::warning("Failed to open file: " + output_path.string());
			return false;
		}
		std::copy(output_data.begin(), output_data.end(), std::ostreambuf_iterator<char>(output_file));

		return true;
	};


	// TODO: Add stream support
	std::filesystem::path Brotli::encode(const std::filesystem::path &path) {
		//std::cout << "Opening file... " << path + "/" + filename << "\n";
		std::ifstream file(path, std::ios::in | std::ios::binary);

		auto file_size = std::filesystem::file_size(path);

		//std::cout << "Allocating buffer...\n";
		// Read file into buffer
		std::vector<uint8_t> input;
		input.reserve(file_size);
		input.assign((std::istreambuf_iterator<char>(file)),
								std::istreambuf_iterator<char>());
		//std::cout << "Compressing file of size < << input.size() << "\n";
		// Compress buffer
		size_t output_size = input.size();
		std::vector<uint8_t> output_data(input.size());

		// TODO: Check return value
		if(BROTLI_FALSE != BrotliEncoderCompress(4, BROTLI_DEFAULT_WINDOW, BROTLI_DEFAULT_MODE, input.size(), input.data(), &output_size, output_data.data())) {
			//std::cout << "Writing to file a size of " << output_size << "\n";
			// Write compressed buffer to file
			std::filesystem::path output_path (path);
			output_path.concat(".br");
			utility::Logger::info("Writing to " + output_path.string());
			if(write_to_file(output_path, output_data)) {
				return output_path;
			} else {
				return {""};
			}
		}

		return {""};
	};

	// std::string GZip::encode(const std::string& path, const std::string& filename) {
	// 	std::fstream file(path + filename, std::ios::in | std::ios::binary);


	// 	libzippp::ZipArchive zip(path + filename + ".gz");
	// 	zip->open(libzippp::ZipArchive::Write);

	// 	zip.setCompressionMethod(libzippp::CompressionMethod::DEFAULT);

	// };

	void FileEncoder::encode_directory(const std::string& path) {
		std::stack<std::filesystem::path> directories {{std::filesystem::path{path}}};

		while(!directories.empty()) {
			const std::filesystem::path current_directory = directories.top();
			directories.pop();
			std::cout << "Encoding directory: " << current_directory << "\n";
			for (auto const& dir_entry : std::filesystem::directory_iterator{current_directory}) {
				if(dir_entry.is_directory()) {
					directories.push(dir_entry.path());
				} else {
					const std::string filename = dir_entry.path().filename().string();
					const std::string extension = dir_entry.path().extension().string();
					std::cout << "Encoding file: " << filename << "\n";

					if(extension == ".html" || extension == ".css" || extension == ".js") {
						const Brotli brotli;
						brotli.encode(std::filesystem::path(current_directory) /= filename);
					}
				}
			}
		}
	};

}; // namespace utility
