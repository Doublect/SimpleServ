#include<gtest/gtest.h>
#include<serverlib/parser.hpp>

#include<string>

using namespace std::literals::string_literals;

template<typename T, typename E>
inline T success(std::expected<T, E> result) {
    EXPECT_TRUE(result.has_value());
    return result.value();
}

#define EXPECT_SUCCESS(result) EXPECT_TRUE(result.has_value())
#define EXPECT_FAILURE(result) EXPECT_FALSE(result.has_value())

const std::string test_request_line = "GET / HTTP/1.1\r\n";

TEST(ParserTest, ParseMessageHeader) {
    std::string input = test_request_line + "Content-Type: text/html\r\n\r\n"s;
 
    auto result = parse_http_request(input);
    EXPECT_SUCCESS(result);
    if(!result.has_value()) {
        return;
    }

    auto headers = result.value().headers();
    EXPECT_EQ(headers["Content-Type"], "text/html");
}

TEST(ParserTest, ParseMessageHeaders) {
    std::string input = test_request_line + "Content-Type: text/html\r\nContent-Length: 100\r\n\r\n"s;

    auto result = parse_http_request(input);
    EXPECT_SUCCESS(result);
    if(!result.has_value()) {
        return;
    }

    auto headers = result.value().headers();

    EXPECT_EQ(headers["Content-Type"], "text/html");
    EXPECT_EQ(headers["Content-Length"], "100");
}

TEST(ParserTest, ParseMessageHeadersWithBody) {
    std::string input = test_request_line + "Content-Type: text/html\r\nContent-Length: 100\r\n\r\n<html><body>Hello, world!</body></html>";

    auto result = parse_http_request(input);
    EXPECT_SUCCESS(result);
    if(!result.has_value()) {
        return;
    }

    auto headers = result.value().headers();

    EXPECT_EQ(headers["Content-Type"], "text/html");
    EXPECT_EQ(headers["Content-Length"], "100");
}

TEST(ParserTest, ParseMessageHeadersWithBodyAndExtra) {
    std::string input = test_request_line + "Content-Type: text/html\r\nContent-Length: 100\r\n\r\n<html><body>Hello, world!</body></html>\r\n\r\n"s;

    auto result = parse_http_request(input);
    EXPECT_SUCCESS(result);
    if(!result.has_value()) {
        return;
    }

    auto headers = result.value().headers();

    EXPECT_EQ(headers["Content-Type"], "text/html");
    EXPECT_EQ(headers["Content-Length"], "100");
}

TEST(ParserTest, MalformedMessageHeaders) {
    std::string input = test_request_line + "\r\nContent-Type: text/html\r\n"s;

    auto result = parse_http_request(input);
    EXPECT_SUCCESS(result);
    if(!result.has_value()) {
        return;
    }

    auto headers = result.value().headers();

    EXPECT_EQ(headers.find("Content-Type"), headers.end());
    EXPECT_EQ(headers.size(), 0);
}

TEST(ParserTest, NoHeaders) {
    std::string input = test_request_line + "\r\n"s;

    auto result = parse_http_request(input);
    EXPECT_SUCCESS(result);
    if(!result.has_value()) {
        return;
    }

    auto headers = result.value().headers();
    EXPECT_EQ(headers.size(), 0);
}

TEST(ParserTest, EmptyInput) {
    std::string input = test_request_line + ""s;

    auto result = parse_http_request(input);
    EXPECT_FAILURE(result);
}