#ifndef WEBSERVER_TCP_SERVER_ASIO_HPP
#define WEBSERVER_TCP_SERVER_ASIO_HPP

#include <serverlib/server.hpp>

#include <boost/asio.hpp>

#define HTTP_PORT 80

class TCPServer : public std::enable_shared_from_this<TCPServer> {
	TCPServer() { this->_port = HTTP_PORT; }
	// TCPServer(std::string&& port_str) { this->port = port_str; }
	// TCPServer(const std::string& port_str) { this->port = std::string(port_str); }

	~TCPServer() { Stop(); }

	virtual void Start();
	virtual void Stop();

private:
	int _port;
	std::atomic<bool> _started;
	boost::asio::io_service::strand _strand;

	struct addrinfo hints, *res;
	struct sockaddr_storage their_addr;
	int sockfd, client_fd;

	void AcceptIncoming();
};

#endif // WEBSERVER_TCP_SERVER_ASIO_HPP