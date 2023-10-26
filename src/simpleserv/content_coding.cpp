#include <simpleserv/utility/content_coding.hpp>

#include <brotli/encode.h>

#include <algorithm>
#include <cstdint>
#include <iostream>
#include <iterator>
#include <filesystem>
#include <fstream>
#include <string>
#include <vector>

namespace utility {

	std::string write_to_file(const std::string& path, const std::string& filename, const std::string& output_prefix, const std::vector<uint8_t>& output_vec) {
		std::string output_path;
		if(output_prefix.empty()) {
			output_path = path + "/" + filename + ".br";
		} else {
			output_path = output_prefix + "/" + path + "/" + filename + ".br";
		}

		// Write to file
		std::ofstream output_file(output_path, std::ios::out | std::ios::binary);
		std::copy(output_vec.begin(), output_vec.end(), std::ostreambuf_iterator<char>(output_file));

		return output_path;
	};


	// TODO: Add stream support
	std::string Brotli::encode(const std::string& path, const std::string& filename, const std::string& output_prefix) {
		//std::cout << "Opening file... " << path + "/" + filename << "\n";
		std::ifstream file(path + "/" + filename, std::ios::in | std::ios::binary);

		auto file_size = std::filesystem::file_size(path + "/" + filename);

		//std::cout << "Allocating buffer...\n";
		// Read file into buffer
		std::vector<uint8_t> input;
		input.reserve(file_size);
		input.assign((std::istreambuf_iterator<char>(file)),
								std::istreambuf_iterator<char>());
		//std::cout << "Compressing file of size < << input.size() << "\n";
		// Compress buffer
		size_t output_size = input.size();
		auto *output = new uint8_t[input.size()];
		// TODO: Check return value
		if(BROTLI_FALSE != BrotliEncoderCompress(4, BROTLI_DEFAULT_WINDOW, BROTLI_DEFAULT_MODE, input.size(), input.data(), &output_size, output)) {
			//std::cout << "Writing to file a size of " << output_size << "\n";
			// Write compressed buffer to file
			const std::vector<uint8_t> output_vec(output, output + output_size);
			return write_to_file(path, filename, output_prefix, output_vec);
		}


		return "";
	};

	// std::string GZip::encode(const std::string& path, const std::string& filename) {
	// 	std::fstream file(path + filename, std::ios::in | std::ios::binary);


	// 	libzippp::ZipArchive zip(path + filename + ".gz");
	// 	zip->open(libzippp::ZipArchive::Write);

	// 	zip.setCompressionMethod(libzippp::CompressionMethod::DEFAULT);

	// };

	void FileEncoder::encode_directory(const std::string& path) {
		const std::filesystem::path dir_path(path);
		std::cout << "Encoding directory: " << dir_path << "\n";
		for (auto const& dir_entry : std::filesystem::directory_iterator{dir_path}) {
			if(dir_entry.is_directory()) {
				encode_directory(dir_entry.path().string());
			} else {
				const std::string filename = dir_entry.path().filename().string();
				const std::string extension = dir_entry.path().extension().string();
				std::cout << "Encoding file: " << filename << "\n";

				if(extension == ".html" || extension == ".css" || extension == ".js") {
					const Brotli brotli;
					brotli.encode(path, filename);
				}
			}
		}
	};

}; // namespace utility
