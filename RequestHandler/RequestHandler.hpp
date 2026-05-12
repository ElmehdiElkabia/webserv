#ifndef REQUEST_HANDLER_HPP
#define REQUEST_HANDLER_HPP

#include <iostream>
#include <ctime>
#include <sstream>
#include <sys/stat.h>
#include <unistd.h>
#include <fstream>
#include <vector>
#include "../HttpRequest.hpp"
#include "../RequestParser/HttpParser.hpp"
#include "../HttpResponse/HttpResponse.hpp"

class RequestHandler
{
private:
	std::string rootDirectory;
	std::string indexFile;
	size_t maxBodySize;

	std::map<std::string, std::string> formData;

	HttpRequest currentRequest;

	std::string fullPath;

	std::string resourceContent;

public:
	RequestHandler();
	HttpResponse HandleRequest(const HttpRequest &request);

private:
	HttpResponse handleGet();
	HttpResponse handlePost();
	HttpResponse handleDelete();

	bool resolvePath();
	bool isDirectory(const std::string &path);
	bool fileExists(const std::string &path);

	bool readFile();
	bool writeFile();
	bool deleteFile();

	std::string getFileExtension(const std::string& path);
	std::string getMimeType(const std::string& extension);
	std::string normalizePath(const std::string &path);

	bool validateBodySize();

	bool parseUrlEncoded();

	bool savePostData();

	std::vector<std::string> split(const std::string &str, char delimiter);

	HttpResponse errorResponse(int code, const std::string& message);
};

#endif