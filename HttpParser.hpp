#ifndef HTTPPARSER_HPP
#define HTTPPARSER_HPP

#include "HttpRequest.hpp"

class HttpParser
{
private:
    std::string buffer;

    HttpRequest currentRequest;

    // Parsing progress (no enum used)
    bool headersParsed;
    bool bodyParsed;

    // Position tracking
    size_t headerEndPos;

public:
    HttpParser();

    // Feed incoming data
    void appendData(const std::string& data);

    // Main parsing function
    // Returns:
    // 0 = incomplete
    // 1 = complete
    // -1 = error
    int parse();

    // Access parsed request
    HttpRequest getRequest() const;

    // Reset parser for next request
    void reset();

private:
    // Internal parsing steps
    bool detectHeadersEnd();
    void parseRequestLine(const std::string& line);
    void parseHeaders(const std::string& headersBlock);
    bool parseBody();

    // Helpers
    std::string extractLine(const std::string& source, size_t& pos, bool& lineComplete);
    void clearProcessedData();
};

#endif