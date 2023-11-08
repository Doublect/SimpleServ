#include <catch2/catch_test_macros.hpp>

#include<simpleserv/utility/url.hpp>

#include<string>

using std::literals::string_literals::operator""s;

TEST_CASE("Path", "[URLTest]") {
    std::string path = "/hello/world";
    std::string query = "foo=bar&baz=qux";

    utility::URL url = utility::from_string(path + "?" + query).value();

    REQUIRE(url.GetPath().compare(path) == 0);
}

TEST_CASE("Query", "[URLTest]") {
    std::string path = "/hello/world";
    std::string query = "foo=bar&baz=qux";

    utility::URL url = utility::from_string(path + "?" + query).value();

    REQUIRE(url.GetQuery("foo"s) == "bar");
    REQUIRE(url.GetQuery("baz"s) == "qux");
}