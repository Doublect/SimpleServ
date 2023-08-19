#include<gtest/gtest.h>
#include<serverlib/parser.hpp>

#include<string>

TEST(ParserTest, ParseMessageHeader) {
    std::string header = "Content-Type: text/html\r\n";

    auto result = std::get<0>(parse_message_headers(header));
    std::cout << "Result: " << result.size() << std::endl;
    for(auto& [key, value] : result) {
        std::cout << key << " " << value << std::endl;
    }
    EXPECT_EQ(result["Content-Type"], "text/html");
}

TEST(ParserTest, ParseMessageHeaders) {
    std::string headers = "Content-Type: text/html\r\nContent-Length: 100\r\n\r\n";

    auto result = std::get<0>(parse_message_headers(headers));

    EXPECT_EQ(result["Content-Type"], "text/html");
    EXPECT_EQ(result["Content-Length"], "100");
}

TEST(ParserTest, ParseMessageHeadersWithBody) {
    std::string headers = "Content-Type: text/html\r\nContent-Length: 100\r\n\r\n<html><body>Hello, world!</body></html>";

    auto result = std::get<0>(parse_message_headers(headers));

    EXPECT_EQ(result["Content-Type"], "text/html");
    EXPECT_EQ(result["Content-Length"], "100");
}

TEST(ParserTest, ParseMessageHeadersWithBodyAndExtra) {
    std::string headers = "Content-Type: text/html\r\nContent-Length: 100\r\n\r\n<html><body>Hello, world!</body></html>\r\n\r\n";

    auto result = std::get<0>(parse_message_headers(headers));

    EXPECT_EQ(result["Content-Type"], "text/html");
    EXPECT_EQ(result["Content-Length"], "100");
}

TEST(ParserTest, MalformedMessageHeaders) {
    std::string headers = "\r\nContent-Type: text/html\r\n";

    auto result = std::get<0>(parse_message_headers(headers));

    EXPECT_EQ(result.find("Content-Type"), result.end());
    EXPECT_EQ(result.size(), 0);
}

TEST(ParserTest, EmptyInput) {
    std::string headers = "";

    auto result = std::get<0>(parse_message_headers(headers));

    EXPECT_EQ(result.size(), 0);
}