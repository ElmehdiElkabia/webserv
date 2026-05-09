#include "RequestHandler.hpp"

RequestHandler::RequestHandler()
	: rootDirectory("./www"),
	  indexFile("index.html"),
	//   maxBodySize(1000000),
	  fullPath("") {}

bool RequestHandler::HandleRequest(const HttpRequest &request)
{
	currentRequest = request;
	if (!resolvePath())
		return false;

	if (currentRequest.method == "GET")
		return handleGet();
	// else if (currentRequest.method == "POST")
	// 	return handlePost();
	else if (currentRequest.method == "DELETE")
		return handleDelete();
	else
	{
		std::cerr << "Unsupported HTTP method: " << currentRequest.method << std::endl;
		return false;
	}
}

std::string RequestHandler::normalizePath(const std::string &path)
{
	std::string normalized = path;
	size_t pos;
	while ((pos = normalized.find("..")) != std::string::npos)
		normalized.erase(pos, 2);
	return normalized;
}

bool RequestHandler::fileExists(const std::string &path)
{
	struct stat buffer;
	return (stat(path.c_str(), &buffer) == 0);
}

bool RequestHandler::isDirectory(const std::string &path)
{
	struct stat buffer;
	if (stat(path.c_str(), &buffer) != 0)
		return false;
	return S_ISDIR(buffer.st_mode);
}

bool RequestHandler::resolvePath()
{
    std::string requestPath = currentRequest.path;

    requestPath = normalizePath(requestPath);

    if (requestPath.empty())
        return false;

    if (requestPath == "/")
        requestPath = "/" + indexFile;

    fullPath = rootDirectory + requestPath;

    return true;
}

bool RequestHandler::handleGet()
{

	if (isDirectory(fullPath))
		fullPath += "/" + indexFile;

	if (!fileExists(fullPath))
	{
		std::cerr << "File not found: " << fullPath << std::endl;
		return false;
	}
	return readFile();
}

bool RequestHandler::readFile()
{
	if (fullPath.empty())
		return false;

	std::ifstream file(fullPath.c_str());
	if (!file.is_open())
	{
		std::cerr << "Failed to open file: " << fullPath << std::endl;
		return false;
	}

	resourceContent.clear();

	std::string line;
	while (std::getline(file, line))
	{
		resourceContent += line;
		if (!file.eof())
			resourceContent += "\n";
	}
	file.close();

	return true;
}


bool RequestHandler::handleDelete()
{
	if(!fileExists(fullPath))
	{
		std::cerr << "File not found: " << fullPath << std::endl;
		return false;
	}
	if (std::remove(fullPath.c_str()) != 0)
	{
		std::cerr << "Failed to delete file: " << fullPath << std::endl;
		return false;
	}
	return true;
}