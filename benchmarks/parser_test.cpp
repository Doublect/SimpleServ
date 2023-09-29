#include <benchmark/benchmark.h>

#include<serverlib/parser.hpp>

#include<string>

void BM_ParseEmptyInput(benchmark::State& state);
void BM_ParseOnlyRequest(benchmark::State& state);
void BM_ParseRequestWithHeaders(benchmark::State& state);

class Setup
{
	Setup()	
	{

	}

public:
	static void PerformSetup()
	{
		static Setup setup;
	}
};

void BM_ParseEmptyInput(benchmark::State& state) {
	Setup::PerformSetup();
	std::string input = "";

	for(auto _ : state) {
		parse_http_request(input);
	}
}

BENCHMARK(BM_ParseEmptyInput);

void BM_ParseOnlyRequest(benchmark::State& state) {
	Setup::PerformSetup();
	std::string input = "GET /a.html HTTP/1.1\r\n\r\n";

	for(auto _ : state) {
		parse_http_request(input);
	}
}

BENCHMARK(BM_ParseOnlyRequest);

void BM_ParseRequestWithHeaders(benchmark::State& state) {
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

	for(auto _ : state) {
		parse_http_request(input);
	}
}

BENCHMARK(BM_ParseRequestWithHeaders);

void BM_ParseRequestLine(benchmark::State& state) {
	Setup::PerformSetup();
	std::string input = "CONNECT /a.html HTTP/1.1\r\n";

	for(auto _ : state) {
		parse_http_request(input);
	}
}

BENCHMARK(BM_ParseRequestLine);