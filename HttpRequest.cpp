#include "HttpRequest.hpp"

HttpRequest::HttpRequest()
	: contentLength(0), isComplete(false), isValid(true), errorCode(0)
{
}


std::string HttpRequest::getUri() const
{
	return path;
}