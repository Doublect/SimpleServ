#include<gtest/gtest.h>
#include<serverlib/url.hpp>

#include<string>

TEST(URLTest, Path) {
    std::string path = "/hello/world";
    std::string query = "foo=bar&baz=qux";

    URL url = from_string(path + "?" + query).value();

    EXPECT_EQ(url.GetPath(), path);
}

TEST(URLTest, Query) {
    std::string path = "/hello/world";
    std::string query = "foo=bar&baz=qux";

    URL url = from_string(path + "?" + query).value();

    EXPECT_EQ(url.GetQuery("foo"), "bar");
    EXPECT_EQ(url.GetQuery("baz"), "qux");
}

int main(int argc, char **argv) {
    testing::InitGoogleTest(&argc, argv);
    return RUN_ALL_TESTS();
}