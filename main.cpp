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
	// 	"GET /style.css HTTP/1.1\r\n"
	// 	"Host: example.com\r\n"
	// 	"User-Agent: Mozilla/5.0\r\n"
	// 	"Accept: text/html\r\n"
	// 	"Connection: keep-alive\r\n"
	// 	"Content-Type: text/css\r\n"
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

		// parser.appendData(
		// "DELETE /doesnotexist.html HTTP/1.1\r\n"
		// "Host: example.com\r\n"
		// "User-Agent: Mozilla/5.0\r\n"
		// "Accept: text/html\r\n"
		// "Connection: keep-alive\r\n"
		// "\r\n");

	parser.appendData(
		"POST /submit HTTP/1.1\r\n"
		"Host: example.com\r\n"
		"Content-Type: application/x-www-form-urlencoded\r\n"
		"Content-Length: 180\r\n"
		"\r\n"
		"filename=en.subject%20%281%29.pdf&filename2=hands-on-network-programming-with-c-learn-socket-programming-in-c-and-write-secure-and-optimized-network-code-9781789349863_compress.pdf"
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
			{
				HttpResponse response = handler.HandleRequest(request);
				std::cout << response.buildResponse() << std::endl;
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