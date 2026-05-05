#include "HttpParser.hpp"
#include "HttpRequest.hpp"

int main()
{
    HttpParser parser;

    // Simulate receiving data in chunks
    parser.appendData(
        "POST /api/user HTTP/1.1\r\n"
        "Host: example.com\r\n"
        "User-Agent: Mozilla/5.0 (X11; Linux x86_64)\r\n"
        "Accept: application/json\r\n"
        "Accept-Language: en-US,en;q=0.9\r\n"
        "Accept-Encoding: gzip, deflate\r\n"
        "Content-Type: application/json\r\n"
        "Content-Length: 77\r\n"
        "Connection: keep-alive\r\n"
        "Cache-Control: no-cache\r\n"
        "Pragma: no-cache\r\n"
        "Origin: http://example.com\r\n"
        "Referer: http://example.com/dashboard\r\n"
        "Authorization: Bearer abc123token\r\n"
        "X-Requested-With: XMLHttpRequest\r\n"
        "\r\n"
        "{\n"
        "  \"username\": \"jdoe\",\n"
        "  \"email\": \"jane.doe@example.com\",\n"
        "  \"active\": true\n"
        "}");
    int result = parser.parse();
    if (result == 1)
    {
        HttpRequest request = parser.getRequest();
        std::cout << "Method: " << request.method << std::endl;
        std::cout << "Path: " << request.path << std::endl;
        std::cout << "Version: " << request.version << std::endl;
        std::cout << "Headers: {" << std::endl;
        for (std::map<std::string, std::string>::const_iterator header = request.headers.begin(); header != request.headers.end(); ++header)
        {
            std::cout << "  " << header->first << ": " << header->second << std::endl;
        }
        std::cout << "}" << std::endl;
        std::cout << "Body: " << request.body << std::endl;
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