#include "HttpResponse.hpp"

HttpResponse::HttpResponse() : version("HTTP/1.1"), statusCode(200), reasonPhrase("OK") {}

void HttpResponse::setStatus(int code)
{
	statusCode = code;
	reasonPhrase = getStatusMessage(code);
}

void HttpResponse::setHeader(const std::string &key, const std::string &value)
{
	headers[key] = value;
}

void HttpResponse::setBody(const std::string &body)
{
	this->body = body;
}

std::string HttpResponse::getStatusMessage(int code) const
{
	switch (code)
	{
	case 200:
		return "OK";
	case 201:
		return "Created";
	case 204:
		return "No Content";
	case 400:
		return "Bad Request";
	case 403:
		return "Forbidden";
	case 404:
		return "Not Found";
	case 405:
		return "Method Not Allowed";
	case 411:
		return "Length Required";
	case 413:
		return "Payload Too Large";
	case 500:
		return "Internal Server Error";
	case 501:
		return "Not Implemented";
	default:
		return "Unknown Status";
	}
}

std::string HttpResponse::buildResponse() const
{
	std::ostringstream statusStream;
	statusStream << statusCode;
	std::string response = version + " " + statusStream.str() + " " + reasonPhrase + "\r\n";

	for (std::map<std::string, std::string>::const_iterator it = headers.begin(); it != headers.end(); ++it)
	{
		response += it->first + ": " + it->second + "\r\n";
	}

	response += "\r\n";
	response += body;

	return response;
}