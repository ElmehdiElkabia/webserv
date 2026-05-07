#ifndef REQUEST_HANDLER_HPP
#define REQUEST_HANDLER_HPP

#include <iostream>
#include <sys/stat.h>
#include <fstream>
#include "../HttpRequest.hpp"
#include "../RequestParser/HttpParser.hpp"

class RequestHandler
{
private:
	std::string rootDirectory;
	std::string indexFile;
	// size_t maxBodySize;

	HttpRequest currentRequest;

	std::string fullPath;

	std::string resourceContent;
public:
	RequestHandler();
	bool HandleRequest(const HttpRequest& request);

private:
	bool handleGet();
	// bool handlePost();
	// bool handleDelete();

	bool resolvePath();
	bool isDirectory(const std::string& path);
	bool fileExists(const std::string& path);

	bool readFile();
	// bool writeFile();
	// bool deleteFile();

	// std::string getFileExtension(const std::string& path);
	std::string normalizePath(const std::string& path);
	
};

#endif