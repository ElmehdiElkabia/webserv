#include "RequestHandler.hpp"
#include <sstream>

RequestHandler::RequestHandler()
	: rootDirectory("./www"),
	  indexFile("index.html"),
	  maxBodySize(1000000),
	  fullPath("")
{
}

HttpResponse RequestHandler::HandleRequest(const HttpRequest &request)
{
	currentRequest = request;
	HttpResponse resp;

	if (!resolvePath())
	{
		resp.setStatus(400);
		resp.setBody("Bad Request: failed to resolve path");
		resp.setHeader("Content-Type", "text/plain");
		return resp;
	}

	if (currentRequest.method == "GET")
		return handleGet();
	else if (currentRequest.method == "POST")
		return handlePost();
	else if (currentRequest.method == "DELETE")
		return handleDelete();
	else
	{
		resp.setStatus(405);
		resp.setBody("Method Not Allowed");
		resp.setHeader("Content-Type", "text/plain");
		return resp;
	}
}

std::string RequestHandler::normalizePath(const std::string &path)
{
	if (path.empty())
		return "";

	if (path.find("..") != std::string::npos)
		return "";

	return path;
}

bool RequestHandler::fileExists(const std::string &path)
{
	return (access(path.c_str(), F_OK) == 0);
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

HttpResponse RequestHandler::handleGet()
{
	if (!resolvePath())
		return errorResponse(400, "Bad Request");

	if (!fileExists(fullPath))
		return errorResponse(404, "Not Found");

	if (isDirectory(fullPath))
	{
		if (fullPath[fullPath.size() - 1] != '/')
			fullPath += "/";

		std::string indexPath = fullPath + indexFile;

		if (!fileExists(indexPath))
			return errorResponse(403, "Forbidden");

		fullPath = indexPath;
	}

	if (access(fullPath.c_str(), R_OK) != 0)
		return errorResponse(403, "Forbidden");

	if (!readFile())
		return errorResponse(500, "Internal Server Error");

	std::string extension = getFileExtension(fullPath);
	std::string mimeType = getMimeType(extension);

	HttpResponse resp;

	resp.setStatus(200);
	resp.setHeader("Content-Type", mimeType);
	resp.setBody(resourceContent);

	return resp;
}

bool RequestHandler::readFile()
{
	if (fullPath.empty())
		return false;

	std::ifstream file(fullPath.c_str(), std::ios::in | std::ios::binary);
	if (!file.is_open())
		return false;

	resourceContent.clear();

	file.seekg(0, std::ios::end);
	std::streampos size = file.tellg();

	if (size < 0)
	{
		file.close();
		return false;
	}

	resourceContent.resize(static_cast<size_t>(size));

	file.seekg(0, std::ios::beg);

	if (size > 0)
		file.read(&resourceContent[0], size);

	file.close();

	return true;
}

bool RequestHandler::deleteFile()
{
	if (fullPath.empty())
		return false;

	if (std::remove(fullPath.c_str()) != 0)
		return false;

	return true;
}

HttpResponse RequestHandler::handleDelete()
{
	if (!resolvePath())
		return errorResponse(400, "Bad Request");

	if (!fileExists(fullPath))
		return errorResponse(404, "Not Found");

	if (isDirectory(fullPath))
		return errorResponse(403, "Forbidden");

	if (!deleteFile())
		return errorResponse(403, "Forbidden");

	HttpResponse resp;

	resp.setStatus(204);
	resp.setHeader("Content-Type", "text/plain");
	resp.setBody("");

	return resp;
}

bool RequestHandler::validateBodySize()
{
	if (currentRequest.body.size() > maxBodySize)
	{
		return false;
	}
	return true;
}

HttpResponse RequestHandler::handlePost()
{
	std::string contentType;
	HttpResponse resp;

	if (currentRequest.body.empty())
		return errorResponse(400, "Bad Request");

	if (!validateBodySize())
		return errorResponse(413, "Payload Too Large");

	if (currentRequest.headers.find("content-type") == currentRequest.headers.end())
		return errorResponse(400, "Missing Content-Type");

	contentType = currentRequest.headers["content-type"];

	if (contentType == "application/x-www-form-urlencoded")
	{
		if (!parseUrlEncoded())
			return errorResponse(400, "Malformed form data");

		if (!savePostData())
			return errorResponse(500, "Internal Server Error");
	}
	else if (contentType.find("multipart/form-data") != std::string::npos)
	{
		if (!savePostData())
			return errorResponse(500, "Internal Server Error");
	}
	else
	{
		return errorResponse(415, "Unsupported Media Type");
	}

	resp.setStatus(201);
	resp.setHeader("Content-Type", "text/plain");
	resp.setBody("Created");

	return resp;
}

bool RequestHandler::parseUrlEncoded()
{
	formData.clear();

	std::vector<std::string> pairs = split(currentRequest.body, '&');

	if (pairs.empty())
		return false;

	for (size_t i = 0; i < pairs.size(); ++i)
	{
		std::vector<std::string> kv = split(pairs[i], '=');

		if (kv.size() != 2)
			return false;

		formData[kv[0]] = kv[1];
	}

	return true;
}
bool RequestHandler::writeFile()
{
	std::ostringstream name;
	name << rootDirectory << "/uploads/post_" << std::time(NULL) << ".txt";

	std::ofstream outFile(name.str().c_str(), std::ios::out | std::ios::binary);

	if (!outFile.is_open())
		return false;

	outFile.write(currentRequest.body.c_str(), currentRequest.body.size());

	outFile.close();

	return true;
}

bool RequestHandler::savePostData()
{
	std::string uploadDir = rootDirectory + "/uploads";

	if (!fileExists(uploadDir))
	{
		if (mkdir(uploadDir.c_str(), 0755) != 0)
			return false;
	}

	return writeFile();
}

std::vector<std::string>
RequestHandler::split(const std::string &str, char delimiter)
{
	std::vector<std::string> tokens;
	std::string token;
	for (size_t i = 0; i < str.size(); ++i)
	{
		if (str[i] == delimiter)
		{
			if (!token.empty())
				tokens.push_back(token);
			token.clear();
		}
		else
			token += str[i];
	}
	if (!token.empty())
		tokens.push_back(token);
	return tokens;
}

std::string RequestHandler::getFileExtension(const std::string &path)
{
	size_t dotPos = path.find_last_of('.');
	if (dotPos == std::string::npos)
		return "";
	return path.substr(dotPos + 1);
}

std::string RequestHandler::getMimeType(const std::string &extension)
{
	if (extension == "html" || extension == "htm")
		return "text/html";
	else if (extension == "css")
		return "text/css";
	else if (extension == "js")
		return "application/javascript";
	else if (extension == "jpg" || extension == "jpeg")
		return "image/jpeg";
	else if (extension == "png")
		return "image/png";
	else if (extension == "gif")
		return "image/gif";
	else if (extension == "txt")
		return "text/plain";
	else
		return "application/octet-stream";
}



HttpResponse RequestHandler::errorResponse(int code, const std::string& message)
{
	HttpResponse resp;

	resp.setStatus(code);
	resp.setHeader("Content-Type", "text/plain");
	resp.setBody(message);

	return resp;
}