#include "HttpParser.hpp"

#include <cstdlib>

HttpParser::HttpParser() : headersParsed(false), bodyParsed(false), headerEndPos(0) {}

HttpRequest HttpParser::getRequest() const
{
    return currentRequest;
}

void HttpParser::reset()
{
    buffer.clear();
    currentRequest = HttpRequest();
    headersParsed = false;
    bodyParsed = false;
    headerEndPos = 0;
}

void HttpParser::appendData(const std::string &data)
{
    buffer.append(data);
}

void HttpParser::parseRequestLine(const std::string &line)
{
    size_t methodEnd = line.find(' ');
    if (methodEnd == std::string::npos)
    {
        currentRequest.isValid = false;
        currentRequest.errorCode = 400;
        return;
    }
    size_t pathEnd = line.find(' ', methodEnd + 1);
    if (pathEnd == std::string::npos)
    {
        currentRequest.isValid = false;
        currentRequest.errorCode = 400;
        return;
    }

    if (line.find(' ', pathEnd + 1) != std::string::npos)
    {
        currentRequest.isValid = false;
        currentRequest.errorCode = 400;
        return;
    }

    currentRequest.method = line.substr(0, methodEnd);
    currentRequest.path = line.substr(methodEnd + 1, pathEnd - methodEnd - 1);
    currentRequest.version = line.substr(pathEnd + 1);

    if (currentRequest.method != "GET" && currentRequest.method != "POST" && currentRequest.method != "DELETE")
    {
        currentRequest.isValid = false;
        currentRequest.errorCode = 405;
        return;
    }
    if (currentRequest.version != "HTTP/1.1")
    {
        currentRequest.isValid = false;
        currentRequest.errorCode = 400;
        return;
    }
    if (currentRequest.path.empty() || currentRequest.path[0] != '/')
    {
        currentRequest.isValid = false;
        currentRequest.errorCode = 400;
        return;
    }
}

bool HttpParser::detectHeadersEnd()
{
    headerEndPos = buffer.find("\r\n\r\n");
    return headerEndPos != std::string::npos;
}

void HttpParser::clearProcessedData()
{
    if (headerEndPos != std::string::npos)
    {
        buffer.erase(0, headerEndPos + 4);
    }
    else
    {
        buffer.clear();
    }
}

std::string HttpParser::extractLine(const std::string &source, size_t &pos, bool &lineComplete)
{
    lineComplete = false;
    if (pos >= source.size())
        return "";

    size_t lineEnd = source.find("\r\n", pos);
    if (lineEnd == std::string::npos)
    {
        std::string line = source.substr(pos);
        pos = source.size();
        lineComplete = true;
        return line;
    }
    std::string line = source.substr(pos, lineEnd - pos);
    pos = lineEnd + 2; // Move past "\r\n"
    lineComplete = true;
    return line;
}

void HttpParser::parseHeaders(const std::string &headersBlock)
{
    size_t pos = 0;
    bool lineComplete = false;
    bool hasHost = false;
    while (pos < headersBlock.size())
    {
        std::string line = extractLine(headersBlock, pos, lineComplete);
        if (!lineComplete)
            return;
        if (line.empty())
            break; // End of headers
        size_t colonPos = line.find(':');
        if (colonPos == std::string::npos)
        {
            currentRequest.isValid = false;
            currentRequest.errorCode = 400;
            return;
        }
        std::string name = line.substr(0, colonPos);
        std::string value = line.substr(colonPos + 1);
        // Trim whitespace
        name.erase(0, name.find_first_not_of(" \t"));
        name.erase(name.find_last_not_of(" \t") + 1);
        value.erase(0, value.find_first_not_of(" \t"));
        value.erase(value.find_last_not_of(" \t") + 1);

        for (size_t i = 0; i < name.size(); ++i)
            name[i] = std::tolower(name[i]);

        if (name == "content-length")
        {
            try
            {
                long long v = std::strtoll(value.c_str(), NULL, 10);
                currentRequest.contentLength = static_cast<int>(v);
            }
            catch (...)
            {
                currentRequest.isValid = false;
                currentRequest.errorCode = 400;
                return;
            }
        }
        else if (name == "host")
        {
            if (value.empty())
            {
                currentRequest.isValid = false;
                currentRequest.errorCode = 400;
                return;
            }
            hasHost = true;
        }
        else if (name == "connection")
        {
            if (value != "keep-alive" && value != "close")
            {
                currentRequest.isValid = false;
                currentRequest.errorCode = 400;
                return;
            }
        }
        currentRequest.headers[name] = value;
    }
    if (!hasHost)
    {
        currentRequest.isValid = false;
        currentRequest.errorCode = 400;
        return;
    }
    headersParsed = true;
}

bool HttpParser::parseBody()
{
    

    if (currentRequest.contentLength > 0)
    {
        if (buffer.size() < currentRequest.contentLength)
            return false; // Body not fully received yet
        currentRequest.body = buffer.substr(0, currentRequest.contentLength);
        clearProcessedData();
    }
    bodyParsed = true;
    return true;
}

int HttpParser::parse()
{
    if (!headersParsed)
    {
        if (!detectHeadersEnd())
            return 0; // Need more data

        std::string requestLineAndHeaders = buffer.substr(0, headerEndPos);
        size_t lineEndPos = requestLineAndHeaders.find("\r\n");
        if (lineEndPos == std::string::npos)
        {
            currentRequest.isValid = false;
            currentRequest.errorCode = 400;
            return -1;
        }
        std::string requestLine = requestLineAndHeaders.substr(0, lineEndPos);
        parseRequestLine(requestLine);
        if (!currentRequest.isValid)
            return -1;

        std::string headersBlock = requestLineAndHeaders.substr(lineEndPos + 2);
        parseHeaders(headersBlock);
        if (!currentRequest.isValid)
            return -1;

        clearProcessedData();
    }

    if (!bodyParsed)
    {
        if (!parseBody())
            return 0; // Need more data
    }

    return 1; // Request fully parsed
}