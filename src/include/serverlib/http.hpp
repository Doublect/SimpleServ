#ifndef HTTP_HPP
#define HTTP_HPP

#include <algorithm>
#include <string>
#include <map>

enum HTTPMethod {
    GET,
    POST,
    PUT,
    DELETE,
    HEAD,
    OPTIONS,
    TRACE,
    CONNECT
};

enum HTTPStatusCode {
    CONTINUE = 100,
    SWITCHING_PROTOCOLS = 101,
    OK = 200,
    CREATED = 201,
    ACCEPTED = 202,
    NO_CONTENT = 204,
    MULTIPLE_CHOICES = 300,
    MOVED_PERMANENTLY = 301,
    FOUND = 302,
    SEE_OTHER = 303,
    NOT_MODIFIED = 304,
    BAD_REQUEST = 400,
    UNAUTHORIZED = 401,
    FORBIDDEN = 403,
    NOT_FOUND = 404,
    INTERNAL_SERVER_ERROR = 500,
    NOT_IMPLEMENTED = 501,
    BAD_GATEWAY = 502,
    SERVICE_UNAVAILABLE = 503
};

enum HTTPVersion {
    HTTP_1_0,
    HTTP_1_1,
    HTTP_2_0
};

class HTTPMessage {
    protected:
        HTTPVersion version_;
        std::map<std::string, std::string> headers_;
        std::string body_;

    public:
        void SetHeader(std::string key, std::string value) {
            key.erase(
                std::remove_if(key.begin(), key.end(),
                    [](char c) { return std::isspace(c); }),
                    key.end());
            value.erase(
                std::remove_if(value.begin(), value.end(),
                    [](char c) { return std::isspace(c); }),
                    value.end());

            headers_[key] = value;
        }

        void SetContent(std::string content) {
            body_ = content;
        }

        HTTPVersion version() const {return version_;}
        std::map<std::string, std::string> headers() const {return headers_;}
        std::string body() const {return body_;}
};

class HTTPRequest : public HTTPMessage {
    private:
        HTTPMethod method_;
        std::string path_;
    public:
        HTTPRequest(HTTPMethod method, std::string path, HTTPVersion version) {
            this->method_ = method;
            this->path_ = path;
            this->version_ = version;
        }

        HTTPMethod method() const {return method_;}
        std::string path() const {return path_;}
};

class HTTPResponse : public HTTPMessage {
    private:
        HTTPStatusCode status_code_;

    public:
        HTTPResponse(HTTPStatusCode status_code, HTTPVersion version) {
            this->status_code_ = status_code;
            this->version_ = version;
            this->body_ = "";
        }

        HTTPStatusCode status_code() const {return status_code_;}
};

static std::string HTTP_METHOD_STRINGS[] = {
    "GET",
    "POST",
    "PUT",
    "DELETE",
    "HEAD",
    "OPTIONS",
    "TRACE",
    "CONNECT"
};

static std::string HTTP_VERSION_STRINGS[] = {
    "HTTP/1.0",
    "HTTP/1.1",
    "HTTP/2.0"
};

static std::string HTTP_RESPONSE_STATUS[] = {
    "200 OK",
    "201 Created",
    "202 Accepted",
    "204 No Content",
    "301 Moved Permanently",
    "302 Found",
    "304 Not Modified",
    "400 Bad Request",
    "401 Unauthorized",
    "403 Forbidden",
    "404 Not Found",
    "500 Internal Server Error",
    "501 Not Implemented",
    "502 Bad Gateway",
    "503 Service Unavailable"
};


std::string request_to_string(const HTTPRequest& request);
std::string response_to_string(const HTTPResponse& response);
HTTPRequest string_to_request(std::string& str);
#endif