#include<gtest/gtest.h>
#include<serverlib/parser.hpp>

#include<string>

template<typename T, typename E>
inline T success(std::expected<T, E> result) {
    EXPECT_TRUE(result.has_value());
    return result.value();
}

#define EXPECT_SUCCESS(result) EXPECT_TRUE(result.has_value())
#define EXPECT_FAILURE(result) EXPECT_FALSE(result.has_value())

TEST(ParserTest, ParseMessageHeader) {
    std::string input = "Content-Type: text/html\r\n\r\n";
 
    auto result = parse_message_headers(input);
    EXPECT_SUCCESS(result);
    if(!result.has_value()) {
        return;
    }

    auto headers = std::get<0>(result.value());
    std::cout << "Result: " << headers.size() << std::endl;
    for(auto& [key, value] : headers) {
        std::cout << key << " " << value << std::endl;
    }
    EXPECT_EQ(headers["Content-Type"], "text/html");
}

TEST(ParserTest, ParseMessageHeaders) {
    std::string input = "Content-Type: text/html\r\nContent-Length: 100\r\n\r\n";

    auto result = parse_message_headers(input);
    EXPECT_SUCCESS(result);
    if(!result.has_value()) {
        return;
    }

    auto headers = std::get<0>(result.value());

    EXPECT_EQ(headers["Content-Type"], "text/html");
    EXPECT_EQ(headers["Content-Length"], "100");
}

TEST(ParserTest, ParseMessageHeadersWithBody) {
    std::string input = "Content-Type: text/html\r\nContent-Length: 100\r\n\r\n<html><body>Hello, world!</body></html>";

    auto result = parse_message_headers(input);
    EXPECT_SUCCESS(result);
    if(!result.has_value()) {
        return;
    }

    auto headers = std::get<0>(result.value());

    EXPECT_EQ(headers["Content-Type"], "text/html");
    EXPECT_EQ(headers["Content-Length"], "100");
}

TEST(ParserTest, ParseMessageHeadersWithBodyAndExtra) {
    std::string input = "Content-Type: text/html\r\nContent-Length: 100\r\n\r\n<html><body>Hello, world!</body></html>\r\n\r\n";

    auto result = parse_message_headers(input);
    EXPECT_SUCCESS(result);
    if(!result.has_value()) {
        return;
    }

    auto headers = std::get<0>(result.value());

    EXPECT_EQ(headers["Content-Type"], "text/html");
    EXPECT_EQ(headers["Content-Length"], "100");
}

TEST(ParserTest, MalformedMessageHeaders) {
    std::string input = "\r\nContent-Type: text/html\r\n";

    auto result = parse_message_headers(input);
    EXPECT_SUCCESS(result);
    if(!result.has_value()) {
        return;
    }

    auto headers = std::get<0>(result.value());

    EXPECT_EQ(headers.find("Content-Type"), headers.end());
    EXPECT_EQ(headers.size(), 0);
}

TEST(ParserTest, NoHeaders) {
    std::string input = "\r\n";

    auto result = parse_message_headers(input);
    EXPECT_SUCCESS(result);
    if(!result.has_value()) {
        return;
    }

    auto headers = std::get<0>(result.value());
    EXPECT_EQ(headers.size(), 0);
}

TEST(ParserTest, EmptyInput) {
    std::string input = "";

    auto result = parse_message_headers(input);
    EXPECT_FAILURE(result);
}