#ifndef WEBSERVER_HTTP_HEADERS_HPP
#define WEBSERVER_HTTP_HEADERS_HPP

#include <map>
#include <string_view>

namespace http {
	enum class FieldType {
		String
	};

	struct Header {
		const std::string_view name;
		FieldType field;
	};

#pragma region General Headers
	constexpr Header CacheControl = Header {
		.name = "Cache-Control",
		.field = FieldType::String
	};

	constexpr Header Connection = Header {
		.name = "Connection",
		.field = FieldType::String
	};

	constexpr Header Date = Header {
		.name = "Date",
		.field = FieldType::String
	};

	constexpr Header Pragma = Header {
		.name = "Pragma",
		.field = FieldType::String
	};

	constexpr Header Trailer = Header {
		.name = "Trailer",
		.field = FieldType::String
	};

	constexpr Header TransferEncoding = Header {
		.name = "Transfer-Encoding",
		.field = FieldType::String
	};

	constexpr Header Upgrade = Header {
		.name = "Upgrade",
		.field = FieldType::String
	};

	constexpr Header Via = Header {
		.name = "Via",
		.field = FieldType::String
	};

	constexpr Header Warning = Header {
		.name = "Warning",
		.field = FieldType::String
	};

#pragma endregion

#pragma region Request Headers
	constexpr Header Accept = Header {
		.name = "Accept",
		.field = FieldType::String
	};

	constexpr Header AcceptCharset = Header {
		.name = "Accept-Charset",
		.field = FieldType::String
	};

	constexpr Header AcceptEncoding = Header {
		.name = "Accept-Encoding",
		.field = FieldType::String
	};

	constexpr Header AcceptLanguage = Header {
		.name = "Accept-Language",
		.field = FieldType::String
	};

	constexpr Header Authorization = Header {
		.name = "Authorization",
		.field = FieldType::String
	};

	constexpr Header Expect = Header {
		.name = "Expect",
		.field = FieldType::String
	};

	constexpr Header From = Header {
		.name = "From",
		.field = FieldType::String
	};

	constexpr Header Host = Header {
		.name = "Host",
		.field = FieldType::String
	};

	constexpr Header IfMatch = Header {
		.name = "If-Match",
		.field = FieldType::String
	};

	constexpr Header IfModifiedSince = Header {
		.name = "If-Modified-Since",
		.field = FieldType::String
	};

	constexpr Header IfNoneMatch  = Header {
		.name = "If-None-Match ",
		.field = FieldType::String
	};

	constexpr Header IfRange = Header {
		.name = "If-Range",
		.field = FieldType::String
	};

	constexpr Header IfUnmodifiedSince = Header {
		.name = "If-Unmodified-Since",
		.field = FieldType::String
	};

	constexpr Header MaxForwards = Header {
		.name = "Max-Forwards",
		.field = FieldType::String
	};

	constexpr Header ProxyAuthorization = Header {
		.name = "Proxy-Authorization",
		.field = FieldType::String
	};

	constexpr Header Range = Header {
		.name = "Range",
		.field = FieldType::String
	};

	constexpr Header Referer = Header {
		.name = "Referer",
		.field = FieldType::String
	};

	constexpr Header TE = Header {
		.name = "TE",
		.field = FieldType::String
	};

	constexpr Header UserAgent = Header {
		.name = "User-Agent",
		.field = FieldType::String
	};

#pragma endregion

#pragma region Response Headers

	constexpr Header AcceptRanges = Header {
		.name = "Accept-Ranges",
		.field = FieldType::String
	};

	constexpr Header Age = Header {
		.name = "Age",
		.field = FieldType::String
	};

	constexpr Header ETag = Header {
		.name = "ETag",
		.field = FieldType::String
	};

	constexpr Header Location = Header {
		.name = "Date",
		.field = FieldType::String
	};

	constexpr Header ProxyAuthenticate = Header {
		.name = "Proxy-Authenticate",
		.field = FieldType::String
	};

	constexpr Header RetryAfter = Header {
		.name = "Retry-After",
		.field = FieldType::String
	};

	constexpr Header Server = Header {
		.name = "Server",
		.field = FieldType::String
	};

	constexpr Header Vary = Header {
		.name = "Vary",
		.field = FieldType::String
	};

	constexpr Header WWWAuthenticate = Header {
		.name = "WWW-Authenticate",
		.field = FieldType::String
	};
#pragma endregion

#pragma region Entity Headers

	constexpr Header Allow = Header {
		.name = "Allow",
		.field = FieldType::String
	};

	constexpr Header ContentEncoding = Header {
		.name = "Content-Encoding",
		.field = FieldType::String
	};

	constexpr Header ContentLanguage = Header {
		.name = "Content-Language",
		.field = FieldType::String
	};

	constexpr Header ContentLength = Header {
		.name = "Content-Length",
		.field = FieldType::String
	};

	constexpr Header ContentLocation = Header {
		.name = "Content-Location",
		.field = FieldType::String
	};

	constexpr Header ContentMD5 = Header {
		.name = "Content-MD5",
		.field = FieldType::String
	};

	constexpr Header ContentRange = Header {
		.name = "Content-Range",
		.field = FieldType::String
	};

	constexpr Header ContentType = Header {
		.name = "Content-Type",
		.field = FieldType::String
	};

	constexpr Header Expires = Header {
		.name = "Expires",
		.field = FieldType::String
	};

	constexpr Header LastModified = Header {
		.name = "Last-Modified",
		.field = FieldType::String
	};
#pragma endregion

	class Headers {
		const Header _general_headers[9] = {
			CacheControl,
			Connection,
			Date,
			Pragma,
			Trailer,
			TransferEncoding,
			Upgrade,
			Via,
			Warning,
		};

		const Header _request_headers[18] = {
			Accept,
			AcceptCharset,
			AcceptEncoding,
			AcceptLanguage,
			Authorization,
			Expect,
			From,
			IfMatch,
			IfModifiedSince,
			IfNoneMatch,
			IfRange,
			IfUnmodifiedSince,
			MaxForwards,
			ProxyAuthorization,
			Range,
			Referer,
			TE,
			UserAgent,
		};

		const Header _response_headers[9]= {
			AcceptRanges,
			Age,
			ETag,
			Location,
			ProxyAuthenticate,
			RetryAfter,
			Server,
			Vary,
			WWWAuthenticate,
		};

		const Header _entity_headers[10] = {
			Allow,
			ContentEncoding,
			ContentLanguage,
			ContentLength,
			ContentLocation,
			ContentMD5,
			ContentRange,
			ContentType,
			Expires,
			LastModified,
		};
	
	};
}
#endif // WEBSERVER_HTTP_HEADERS_HPP