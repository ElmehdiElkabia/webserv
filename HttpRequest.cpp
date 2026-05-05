#include "HttpRequest.hpp"

HttpRequest::HttpRequest()
	: contentLength(0), isComplete(false), isValid(true), errorCode(0)
{
}