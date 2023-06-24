#include<gtest/gtest.h>
#include<serverlib/uri.hpp>

#include<string>

TEST(URLTest, Path) {
    std::string path = "/hello/world";
    std::string query = "foo=bar&baz=qux";

    URI uri = from_string(path + "?" + query);

    EXPECT_EQ(uri.GetPath(), path);
}

TEST(URLTest, Query) {
    std::string path = "/hello/world";
    std::string query = "foo=bar&baz=qux";

    URI uri = from_string(path + "?" + query);

    EXPECT_EQ(uri.GetQuery("foo"), "bar");
    EXPECT_EQ(uri.GetQuery("baz"), "qux");
}

int main(int argc, char **argv) {
    testing::InitGoogleTest(&argc, argv);
    return RUN_ALL_TESTS();
}