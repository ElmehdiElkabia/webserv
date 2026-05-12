#ifndef HTTPRESPONSE_HPP
#define HTTPRESPONSE_HPP



#include <iostream>
#include <map>
#include <sstream>

class HttpResponse
{
private:
    std::string version;
    int statusCode;
    std::string reasonPhrase;
    std::map<std::string, std::string> headers;
    std::string body;

public:
    HttpResponse();

    void setStatus(int code);
    void setHeader(const std::string& key, const std::string& value);
    void setBody(const std::string& body);

    std::string buildResponse() const;

private:
    std::string getStatusMessage(int code) const;
};


#endif