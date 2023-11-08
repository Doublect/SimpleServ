#include <catch2/catch_test_macros.hpp>

#include <simpleserv/http/parser.hpp>

#include <string>

using namespace std::literals::string_literals;
using namespace http;

const std::string test_request_line = "GET / HTTP/1.1\r\n";

TEST_CASE("ParseMessageHeader", "[ParserTest]") {
    std::string input = test_request_line + "Content-Type: text/html\r\n\r\n"s;
 
    auto result = http::parse_http_request(input);
    REQUIRE(result.has_value() == true);
    if(!result.has_value()) {
        return;
    }

    auto headers = result.value().headers();
		REQUIRE(headers.size() == 1);
    REQUIRE(headers["Content-Type"] == "text/html");
}

TEST_CASE("ParseMessageHeaders", "[ParserTest]") {
    std::string input = test_request_line + "Content-Type: text/html\r\nContent-Length: 100\r\n\r\n"s;

    auto result = parse_http_request(input);
    REQUIRE(result);
    if(!result.has_value()) {
        return;
    }

    auto headers = result.value().headers();

    REQUIRE(headers["Content-Type"] == "text/html");
    REQUIRE(headers["Content-Length"] == "100");
}

TEST_CASE("ParseMessageHeadersWithBody", "[ParserTest]") {
    std::string input = test_request_line + "Content-Type: text/html\r\nContent-Length: 100\r\n\r\n<html><body>Hello, world!</body></html>";

    auto result = parse_http_request(input);
    REQUIRE(result);
    if(!result.has_value()) {
        return;
    }

    auto headers = result.value().headers();

    REQUIRE(headers["Content-Type"] == "text/html");
    REQUIRE(headers["Content-Length"] == "100");
		REQUIRE(result.value().body() == "<html><body>Hello, world!</body></html>");
}

TEST_CASE("ParseMessageHeadersWithBodyAndExtra", "[ParserTest]") {
    std::string input = test_request_line + "Content-Type: text/html\r\nContent-Length: 100\r\n\r\n<html><body>Hello, world!</body></html>\r\n\r\n"s;

    auto result = parse_http_request(input);
    REQUIRE(result);
    if(!result.has_value()) {
        return;
    }

    auto headers = result.value().headers();

    REQUIRE(headers["Content-Type"] == "text/html");
    REQUIRE(headers["Content-Length"] == "100");
}

TEST_CASE("MalformedMessageHeaders", "[ParserTest]") {
    std::string input = test_request_line + "Content\r\n-Type: text/html\r\n"s;

    auto result = parse_http_request(input);
    REQUIRE_FALSE(result);
}

TEST_CASE("NoHeaders", "[ParserTest]") {
    std::string input = test_request_line + "\r\n"s;

    auto result = parse_http_request(input);
    REQUIRE(result);
    if(!result.has_value()) {
        return;
    }

    auto headers = result.value().headers();
    REQUIRE(headers.size() == 0);
}

TEST_CASE("EmptyInput", "[ParserTest]") {
    std::string input = test_request_line + ""s;

    auto result = parse_http_request(input);
    REQUIRE_FALSE(result);
}