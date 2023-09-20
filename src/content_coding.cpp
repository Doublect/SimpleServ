#include "./include/serverlib/content_coding.hpp"

#include <libzippp.h>
#include <brotli/encode.h>

#include <algorithm>
#include <iostream>
#include <iterator>
#include <fstream>

// TODO: Add stream support
std::string Brotli::encode(const std::string& path, const std::string& filename, const std::string& output_prefix) {
	std::cout << "Opening file... " << path + "/" + filename << std::endl;
	std::ifstream file(path + "/" + filename, std::ios::in | std::ios::binary);

	auto file_size = std::filesystem::file_size(path + "/" + filename);

	std::cout << "Allocating buffer..." << std::endl;
	// Read file into buffer
	std::vector<uint8_t> input;
	input.reserve(file_size);
	input.assign((std::istreambuf_iterator<char>(file)),
				       std::istreambuf_iterator<char>());
	std::cout << "Compressing file of size " << input.size() << std::endl;
	// Compress buffer
	size_t output_size = input.size();
	u_int8_t* output = new u_int8_t[input.size()];
	// TODO: Check return value
	if(BROTLI_FALSE != BrotliEncoderCompress(4, BROTLI_DEFAULT_WINDOW, BROTLI_DEFAULT_MODE, input.size(), input.data(), &output_size, output)) {
		std::cout << "Writing to file a size of " << output_size << std::endl;
		// Write compressed buffer to file
		std::vector<uint8_t> output_vec(output, output + output_size);
		std::string output_path;
		
		if(output_prefix == "") {
			output_path = path + "/" + filename + ".br";
		} else {
			output_path = output_prefix + "/" + path + "/" + filename + ".br";
		}

		std::ofstream output_file(output_path, std::ios::out | std::ios::binary);
		std::copy(output_vec.begin(), output_vec.end(), std::ostreambuf_iterator<char>(output_file));

		return output_path;
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
	std::cout << "Encoding directory: " << dir_path << std::endl;
	for (auto const& dir_entry : std::filesystem::directory_iterator{dir_path}) {
		if(dir_entry.is_directory()) {
			encode_directory(dir_entry.path().string());
		} else {
			std::string filename = dir_entry.path().filename().string();
			std::string extension = dir_entry.path().extension().string();
			std::cout << "Encoding file: " << filename << std::endl;

			if(extension == ".html" || extension == ".css" || extension == ".js") {
				Brotli brotli;
				brotli.encode(path, filename);
			}
		}
	}
};
