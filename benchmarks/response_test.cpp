#include <benchmark/benchmark.h>

#include<serverlib/socket_layer/http_handler.hpp>

#include<string>

void BM_EmptyRequest(benchmark::State& state);
void BM_SimpleFileRequest(benchmark::State& state);
void BM_SimpleFileRequestWithCompression(benchmark::State& state);
void BM_SimpleFileRequestWithHeaders(benchmark::State& state);

class Setup
{
	Setup()	
	{
		file_manager.construct_file_descriptors("../webdir");
		file_manager.generate_encoded_files();
	}

public:
	static void PerformSetup()
	{
		static Setup setup;
	}
};

void BM_EmptyRequest(benchmark::State& state) {
	Setup::PerformSetup();
	std::string input = "";
	HTTPHandler handler;

	for(auto _ : state) {
		handler.handle(input);
	}
}

BENCHMARK(BM_EmptyRequest);

void BM_SimpleFileRequest(benchmark::State& state) {
	Setup::PerformSetup();
	std::string input = "GET /a.html HTTP/1.1\r\n\r\n";
	HTTPHandler handler;

	for(auto _ : state) {
		handler.handle(input);
	}
}

BENCHMARK(BM_SimpleFileRequest);

void BM_SimpleFileRequestWithCompression(benchmark::State& state) {
	std::string input = "GET /a.html HTTP/1.1\r\nAccept-Encoding: gzip\r\n\r\n";
	HTTPHandler handler;

	for(auto _ : state) {
		handler.handle(input);
	}
}

BENCHMARK(BM_SimpleFileRequestWithCompression);

void BM_SimpleFileRequestWithHeaders(benchmark::State& state) {
	Setup::PerformSetup();
	std::string input = "GET /a.html HTTP/1.1\r\n\
Host: localhost\r\n\
User-Agent: Mozilla/5.0 (X11; Linux x86_64; rv:109.0) Gecko/20100101 Firefox/118.0\r\n\
Accept: text/html,application/xhtml+xml,application/xml;q=0.9,image/avif,image/webp,*/*;q=0.8\r\n\
Accept-Language: en-US,en;q=0.5\r\n\
Accept-Encoding: gzip, deflate, br\r\n\
DNT: 1\r\n\
Connection: keep-alive\r\n\
Upgrade-Insecure-Requests: 1\r\n\
Sec-Fetch-Dest: document\r\n\
Sec-Fetch-Mode: navigate\r\n\
Sec-Fetch-Site: none\r\n\
Sec-Fetch-User: ?1\r\n";
	HTTPHandler handler;

	for(auto _ : state) {
		handler.handle(input);
	}
}

BENCHMARK(BM_SimpleFileRequestWithHeaders);