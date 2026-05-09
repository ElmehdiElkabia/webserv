#include "RequestParser/HttpParser.hpp"
#include "HttpRequest.hpp"
#include "RequestHandler/RequestHandler.hpp"

int main()
{
	HttpParser parser;

	// Simulate receiving data in chunks
	// parser.appendData(
	// 	"GET /index.html HTTP/1.1\r\n"
	// 	"Host: example.com\r\n"
	// 	"User-Agent: Mozilla/5.0\r\n"
	// 	"Accept: text/html\r\n"
	// 	"Connection: keep-alive\r\n"
	// 	"\r\n");

	// parser.appendData(
	// 	"GET / HTTP/1.1\r\n"
	// 	"Host: example.com\r\n"
	// 	"User-Agent: Mozilla/5.0\r\n"
	// 	"Accept: text/html\r\n"
	// 	"Connection: keep-alive\r\n"
	// 	"\r\n");

	// parser.appendData(
	// 	"GET /doesnotexist.html HTTP/1.1\r\n"
	// 	"Host: example.com\r\n"
	// 	"User-Agent: Mozilla/5.0\r\n"
	// 	"Accept: text/html\r\n"
	// 	"Connection: keep-alive\r\n"
	// 	"\r\n");

	// parser.appendData(
	// 	"DELETE /uploads/data.txt HTTP/1.1\r\n"
	// 	"Host: example.com\r\n"
	// 	"User-Agent: Mozilla/5.0\r\n"
	// 	"Accept: text/html\r\n"
	// 	"Connection: keep-alive\r\n"
	// 	"\r\n");

	parser.appendData(
		"POST /submit HTTP/1.1\r\n"
		"Host: example.com\r\n"
		"Content-Type: application/x-www-form-urlencoded\r\n"
		"Content-Length: 65\r\n"
		"\r\n"
		"username=john&age=22"
		"\r\n"
		"email=john%40example.com"
		"\r\n"
		"city=New%20York"
		"\r\n");

	int result = parser.parse();
	if (result == 1)
	{
		HttpRequest request = parser.getRequest();
		// std::cout << "Method: " << request.method << std::endl;
		// std::cout << "Path: " << request.path << std::endl;
		// std::cout << "Version: " << request.version << std::endl;
		// std::cout << "Headers: {" << std::endl;
		// for (std::map<std::string, std::string>::const_iterator header = request.headers.begin(); header != request.headers.end(); ++header)
		// {
		// 	std::cout << "  " << header->first << ": " << header->second << std::endl;
		// }
		// std::cout << "}" << std::endl;
		// std::cout << "Body: " << request.body << std::endl;

		RequestHandler handler;
		if (handler.HandleRequest(request))
		{
			std::cout << "Request handled successfully" << std::endl;
		}
		else
		{
			std::cerr << "Failed to handle request" << std::endl;
		}
	}
	else if (result == -1)
	{
		std::cerr << "Error parsing request" << std::endl;
	}
	else
	{
		std::cerr << "Request incomplete, waiting for more data" << std::endl;
	}

	return 0;
}