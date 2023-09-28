#include "include/serverlib/message.hpp"

#include <iterator>
#include <map>
#include <stdexcept>
#include <string>

void HTTPMessage::SetHeader(std::string key, std::string value) {
	key.erase(std::remove_if(key.begin(), key.end(),
														[](char c) { return std::isspace(c); }),
						key.end());
	value.erase(std::remove_if(value.begin(), value.end(),
															[](char c) { return std::isspace(c); }),
							value.end());

	headers_[key] = value;
};