#ifndef URL_HPP
#define URL_HPP

#include <serverlib/utility/expected.hpp>

#include <string>
#include <map>

class URL {
	std::string path;
	std::map<std::string, std::string> query;

	public:
		URL(std::string url_path, std::map<std::string, std::string> query_params) : path(url_path), query(query_params) {}
		~URL() {}
		std::string GetPath() const {
			return path;
		}

		std::map<std::string, std::string> GetQuery() const {
			return query;
		}

		std::string GetQuery(std::string key) {
			return query[key];
		}
};

auto from_string(const std::string& str) -> utility::expected<URL, std::string>;

#endif