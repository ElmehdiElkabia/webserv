#include "RequestHandler.hpp"

RequestHandler::RequestHandler()
	: rootDirectory("./www"),
	  indexFile("index.html"),
	  //   maxBodySize(1000000),
	  fullPath("")
{
}

bool RequestHandler::HandleRequest(const HttpRequest &request)
{
	currentRequest = request;
	if (!resolvePath())
		return false;

	if (currentRequest.method == "GET")
		return handleGet();
	else if (currentRequest.method == "POST")
		return handlePost();
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

bool RequestHandler::deleteFile()
{
    // Empty path protection
    if (fullPath.empty())
        return false;

    // Check existence
    if (!fileExists(fullPath))
    {
        std::cerr << "File not found: "
                  << fullPath << std::endl;
        return false;
    }

    // Check write permission
    if (access(fullPath.c_str(), W_OK) != 0)
    {
        std::cerr << "Permission denied: "
                  << fullPath << std::endl;
        return false;
    }

    // Delete file
    if (std::remove(fullPath.c_str()) != 0)
    {
        std::cerr << "Failed to delete file: "
                  << fullPath << std::endl;
        return false;
    }

    return true;
}

bool RequestHandler::handleDelete()
{
    // Resolve filesystem path first
    if (!resolvePath())
    {
        std::cerr << "Failed to resolve path" << std::endl;
        return false;
    }

    // Prevent deleting directories
    if (isDirectory(fullPath))
    {
        std::cerr << "Cannot delete directory: "
                  << fullPath << std::endl;
        return false;
    }

    // Delete resource
    return deleteFile();
}

bool RequestHandler::validateBodySize()
{
	if (currentRequest.body.size() > maxBodySize)
	{
		std::cerr << "Request body too large: " << currentRequest.body.size() << " bytes" << std::endl;
		return false;
	}
	return true;
}

bool RequestHandler::handlePost()
{
	// Empty body check
	if (currentRequest.body.empty())
	{
		std::cerr << "POST request missing body" << std::endl;
		return false;
	}

	// Body size protection
	if (currentRequest.body.size() > maxBodySize)
	{
		std::cerr << "POST body exceeds maxBodySize" << std::endl;
		return false;
	}

	if (currentRequest.ContentType == "application/x-www-form-urlencoded")
	{
		// Parse form data
		if (!parseUrlEncoded())
		{
			std::cerr << "Failed to parse urlencoded body" << std::endl;
			return false;
		}

		// Save parsed/raw POST data
		if (!savePostData())
		{
			std::cerr << "Failed to save POST data" << std::endl;
			return false;
		}
	}

	return true;
}

bool RequestHandler::parseUrlEncoded()
{
	std::string body = currentRequest.body;
	std::vector<std::string> pairs = split(body, '&');
	if (pairs.empty())
		return false;
	for (size_t i = 0; i < pairs.size(); ++i)
	{
		std::vector<std::string> kv = split(pairs[i], '=');
		if (kv.size() != 2)
			continue;
		std::string key = kv[0];
		std::string value = kv[1];

		formData[key] = value;
	}
	return true;
}
bool RequestHandler::writeFile()
{
	// Example filename
	std::string uploadPath = rootDirectory + "/uploads/data.txt";
	// Open output file
	std::ofstream outFile(uploadPath.c_str(), std::ios::out | std::ios::binary);

	// Check open success
	if (!outFile.is_open())
	{
		std::cerr << "Failed to open file for writing: "
				  << uploadPath << std::endl;
		return false;
	}

	// Write POST body into file
	outFile << currentRequest.body;

	// Close file
	outFile.close();

	return true;
}

bool RequestHandler::savePostData()
{
	// Upload directory
	std::string uploadDir = rootDirectory + "/uploads";

	// Create uploads directory if missing
	if (!fileExists(uploadDir))
	{
		if (mkdir(uploadDir.c_str(), 0755) != 0)
		{
			std::cerr << "Failed to create upload directory: "
					  << uploadDir << std::endl;
			return false;
		}
	}

	// Save POST body into file
	if (!writeFile())
	{
		std::cerr << "Failed to save POST data" << std::endl;
		return false;
	}

	return true;
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