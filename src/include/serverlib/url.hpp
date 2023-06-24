#ifndef URL_HPP
#define URL_HPP

#include <expected>
#include <string>
#include <map>
#include <memory>

class URL {
    std::string path;
    std::map<std::string, std::string> query;

    public:
        URL(std::string path, std::map<std::string, std::string> query) : path(path), query(query) {}
        ~URL() {}
        std::string GetPath() {
            return path;
        }

        std::map<std::string, std::string> GetQuery() {
            return query;
        }

        std::string GetQuery(std::string key) {
            return query[key];
        }
};

auto from_string(std::string str) -> std::expected<URL, std::string>;

#endif