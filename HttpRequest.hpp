#ifndef HTTPREQUEST_HPP
#define HTTPREQUEST_HPP

#include <iostream>
#include <map>

class HttpRequest
{
public:
    // Request line
    std::string method;
    std::string path;
    std::string version;

    // Headers & body
    std::map<std::string, std::string> headers;
    std::string body;

	std::string ContentType;
	std::string TransferEncoding;
	std::string Host;
    size_t      contentLength;

    // Parsing / validation state
    bool        isComplete;
    bool        isValid;
    int         errorCode;

public:
    HttpRequest();
	std::string getUri() const;
};

#endif