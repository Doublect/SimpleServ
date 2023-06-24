#ifndef URL_HPP
#define URL_HPP

#include <string>
#include <map>

class URI {
    std::string path;
    std::map<std::string, std::string> query;

    public:
        URI(std::string path, std::map<std::string, std::string> query) : path(path), query(query) {}

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

URI from_string(std::string str);

#endif