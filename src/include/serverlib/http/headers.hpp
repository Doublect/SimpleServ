#ifndef WEBSERVER_HTTP_HEADERS_HPP
#define WEBSERVER_HTTP_HEADERS_HPP

#include <string>

namespace http {
	enum class FieldType {
		String
	};

	struct Header {
		std::string name;
		FieldType field;
	};

#pragma region General Headers
	class CacheControl : public Header {
		std::string name = "Cache-Control";
		FieldType field = FieldType::String;
	};

	class Connection : public Header {
		std::string name = "Connection";
		FieldType field = FieldType::String;
	};

	class Date : public Header {
		std::string name = "Date";
		FieldType field = FieldType::String;
	};

	class Pragma : public Header {
		std::string name = "Pragma";
		FieldType field = FieldType::String;
	};

	class Trailer : public Header {
		std::string name = "Trailer";
		FieldType field = FieldType::String;
	};

	class TransferEncoding : public Header {
		std::string name = "Transfer-Encoding";
		FieldType field = FieldType::String;
	};

	class Upgrade : public Header {
		std::string name = "Upgrade";
		FieldType field = FieldType::String;
	};

	class Via : public Header {
		std::string name = "Via";
		FieldType field = FieldType::String;
	};

	class Warning : public Header {
		std::string name = "Warning";
		FieldType field = FieldType::String;
	};

#pragma endregion

#pragma region Request Headers
	class Accept : public Header {
		std::string name = "Accept";
		FieldType field = FieldType::String;
	};

	class AcceptCharset : public Header {
		std::string name = "Accept-Charset";
		FieldType field = FieldType::String;
	};

	class AcceptEncoding : public Header {
		std::string name = "Accept-Encoding";
		FieldType field = FieldType::String;
	};

	class AcceptLanguage : public Header {
		std::string name = "Accept-Language";
		FieldType field = FieldType::String;
	};

	class Authorization : public Header {
		std::string name = "Authorization";
		FieldType field = FieldType::String;
	};

	class Expect : public Header {
		std::string name = "Expect";
		FieldType field = FieldType::String;
	};

	class From : public Header {
		std::string name = "From";
		FieldType field = FieldType::String;
	};

	class Host : public Header {
		std::string name = "Host";
		FieldType field = FieldType::String;
	};

	class IfMatch : public Header {
		std::string name = "If-Match";
		FieldType field = FieldType::String;
	};

	class IfModifiedSince : public Header {
		std::string name = "If-Modified-Since";
		FieldType field = FieldType::String;
	};

	class IfNoneMatch  : public Header {
		std::string name = "If-None-Match ";
		FieldType field = FieldType::String;
	};

	class IfRange : public Header {
		std::string name = "If-Range";
		FieldType field = FieldType::String;
	};

	class IfUnmodifiedSince : public Header {
		std::string name = "If-Unmodified-Since";
		FieldType field = FieldType::String;
	};

	class MaxForwards : public Header {
		std::string name = "Max-Forwards";
		FieldType field = FieldType::String;
	};

	class ProxyAuthorization : public Header {
		std::string name = "Proxy-Authorization";
		FieldType field = FieldType::String;
	};

	class Range : public Header {
		std::string name = "Range";
		FieldType field = FieldType::String;
	};

	class Referer : public Header {
		std::string name = "Referer";
		FieldType field = FieldType::String;
	};

	class TE : public Header {
		std::string name = "TE";
		FieldType field = FieldType::String;
	};

	class UserAgent : public Header {
		std::string name = "User-Agent";
		FieldType field = FieldType::String;
	};

#pragma endregion

#pragma region Response Headers

	class AcceptRanges : public Header {
		std::string name = "Accept-Ranges";
		FieldType field = FieldType::String;
	};

	class Age : public Header {
		std::string name = "Age";
		FieldType field = FieldType::String;
	};

	class ETag : public Header {
		std::string name = "ETag";
		FieldType field = FieldType::String;
	};

	class Location : public Header {
		std::string name = "Date";
		FieldType field = FieldType::String;
	};

	class ProxyAuthenticate : public Header {
		std::string name = "Proxy-Authenticate";
		FieldType field = FieldType::String;
	};

	class RetryAfter : public Header {
		std::string name = "Retry-After";
		FieldType field = FieldType::String;
	};

	class Server : public Header {
		std::string name = "Server";
		FieldType field = FieldType::String;
	};

	class Vary : public Header {
		std::string name = "Vary";
		FieldType field = FieldType::String;
	};

	class WWWAuthenticate : public Header {
		std::string name = "WWW-Authenticate";
		FieldType field = FieldType::String;
	};
#pragma endregion

#pragma region Entity Headers

	class Allow : public Header {
		std::string name = "Allow";
		FieldType field = FieldType::String;
	};

	class ContentEncoding : public Header {
		std::string name = "Content-Encoding";
		FieldType field = FieldType::String;
	};

	class ContentLanguage : public Header {
		std::string name = "Content-Language";
		FieldType field = FieldType::String;
	};

	class ContentLength : public Header {
		std::string name = "Content-Length";
		FieldType field = FieldType::String;
	};

	class ContentLocation : public Header {
		std::string name = "Content-Location";
		FieldType field = FieldType::String;
	};

	class ContentMD5 : public Header {
		std::string name = "Content-MD5";
		FieldType field = FieldType::String;
	};

	class ContentRange : public Header {
		std::string name = "Content-Range";
		FieldType field = FieldType::String;
	};

	class ContentType : public Header {
		std::string name = "Content-Type";
		FieldType field = FieldType::String;
	};

	class Expires : public Header {
		std::string name = "Expires";
		FieldType field = FieldType::String;
	};

	class LastModified : public Header {
		std::string name = "Last-Modified";
		FieldType field = FieldType::String;
	};
#pragma endregion
}

#endif // WEBSERVER_HTTP_HEADERS_HPP