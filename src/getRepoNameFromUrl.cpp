
#include "definitions.h"

#include <string>

std::string getRepoNameFromUrl(const std::string& url)
{
	std::string repoName;
	size_t pos = 0;
	if ((pos = url.find_last_of("/")) == std::string::npos) {
		return repoName;  // Return an empty string to singal an error
	}
	repoName = url.substr(pos + 1, (url.size() - pos - 1));
	
	return repoName;
}