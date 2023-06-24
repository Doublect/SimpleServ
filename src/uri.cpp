#include "include/serverlib/uri.hpp"

std::string path(std::string str) {
    int query_index = str.find("?");
    if(query_index != std::string::npos) {
        return str.substr(0, query_index);
    }

    return str;
}

std::map<std::string, std::string> searchpart(std::string str, size_t start) {
    std::map<std::string, std::string> query;

    int query_index = start;

    while(true) {
        int eq_pos = str.find("=", query_index);
        int amp_pos = str.find("&", query_index);

        amp_pos = amp_pos == std::string::npos ? str.length() : amp_pos;
        
        if(eq_pos == std::string::npos) {
            break;
        }

        std::string key = str.substr(query_index, eq_pos - query_index);
        std::string value = str.substr(eq_pos + 1, amp_pos - eq_pos - 1);

        query[key] = value;

        query_index = amp_pos + 1;
    }

    return query;
}

URI from_string(std::string str) {
    std::string path_str = path(str);
    std::map<std::string, std::string> query = searchpart(str, path_str.length() + 1);

    return URI(path_str, query);
}