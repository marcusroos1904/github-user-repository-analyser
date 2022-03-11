
#include "definitions.h"
#include <string>
#include <algorithm>
#include <curl/curl.h>
#include <curl/easy.h>
#include <iostream>
#include <vector>

#define GITHUB_USERNAME "alexkarlssonn"  // Used to increase the API rate-limit


static size_t curl_to_string(void* ptr, size_t size, size_t nmemb, void* data)
{
    std::string* str = (std::string*) data;
    char* sptr = (char*) ptr;
    for (int x = 0; x < (size * nmemb); ++x) {
        (*str) += sptr[x];
    }

    return size * nmemb;
}

static std::string curl_request(const std::string& url, const std::string& header)
{
	// Initiate curl
	std::string pagedata;
	CURL* curl = curl_easy_init();
	if (!curl) {
		return pagedata;  // Error
	}

	// Setup curl
	std::cout.setstate(std::ios::failbit);
    curl_easy_setopt(curl, CURLOPT_URL, url.c_str());
    // If a header is given, then setup the authentication options for curl
    if (!header.empty()) {
    	curl_easy_setopt(curl, CURLOPT_HTTPAUTH, (long)CURLAUTH_BASIC);
    	curl_easy_setopt(curl, CURLOPT_USERNAME, GITHUB_USERNAME);
    	curl_easy_setopt(curl, CURLOPT_USERAGENT, header.c_str());
    }
    curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, curl_to_string);
    curl_easy_setopt(curl, CURLOPT_WRITEDATA, &pagedata);

    // Use curl to send a request for the html document
    CURLcode response = curl_easy_perform(curl);
    if (CURLE_OK != response) {
        return pagedata;  // Error
    }
	
	// Cleanup curl and reset std::cout
	curl_easy_cleanup(curl);
	std::cout.clear();
	return pagedata;
}


/* ------------------------------------------------------------------------------------------------
 * This function tries to lookup the author name that the given GitHub user has when making commits to the given repository
 * The GitHub user is specified as the parameter: username
 * The repository is specified as the parameter: repoUrl
 * 
 * Return the author name on success, and an empty string on failure
 * ------------------------------------------------------------------------------------------------ */
std::string getAuthorname(const std::string username, const std::string& repoUrl)
{
	std::string authorname;

	// Construct the URL to list some of the commits that the given user has made
	std::string commitsUrl = repoUrl;
	commitsUrl = commitsUrl + "/commits?author=" + username;

	// Use curl to send a request for the html document
	std::string pagedata = curl_request(commitsUrl, "");
	if (pagedata.empty()) {
		return authorname;
	}

	// Parse the html document and attempt to extract the commit id's for the most recent commits the user has made
	size_t pos = -1;
	size_t start, end;
	std::vector<std::string> commits;
	while ((pos = pagedata.find("aria-label=\"View commit details\"", pos + 1)) != std::string::npos)
	{
		// Find the start end end positions for the current commit hash
		start = 0;
		end = 0;
		for (int i = pos; i > (pos - 64); --i) {
			if (end == 0 && pagedata[i] == '"') {
				end = i;
			}
			if (end != 0 && pagedata[i] == '/') {
				start = i + 1;
				break;
			}
		}

		// Add the commit id to the vector
		if (start > 0 && end > 0) {
			std::string substr = pagedata.substr(start, end - start);
			commits.push_back(substr);
		}
	}
	
	// The vector will be empty if an invalid username was used, or if no commits were found
	if (commits.empty()) {
		return authorname;
	}


	// Extract the name of the repository
	std::string copy = repoUrl;
	if ((pos = copy.find_last_of("/")) == std::string::npos) 
		return authorname;  // Error
	std::string repoName = copy.substr(pos + 1, (copy.size() - pos - 1));
	
	// Extract the name of the repository owner 
	copy.erase(pos, std::string::npos);
	if ((pos = copy.find_last_of("/")) == std::string::npos) 
		return authorname;  // Error
	std::string repoOwner = copy.substr(pos + 1, (copy.size() - pos - 1));

	// Construct the base API url
	std::string baseApiUrl = "https://api.github.com/repos/";
	baseApiUrl = baseApiUrl + repoOwner + "/" + repoName + "/commits/";


	// Loop through the commits (stop when a valid commit author is found)
	for (int i = 0; i < commits.size(); i++) 
	{
		// Construct the current API url
		std::string apiUrl = baseApiUrl;
		apiUrl += commits[i];
		
		// Use curl to send the API request
		std::string jsondata = curl_request(apiUrl, "Accept: application/vnd.github.v3+json");
		if (jsondata.empty()) {
			continue;
		}

		// Parse the jsondata and attempt to extract the author name of the commit
		if ((pos = jsondata.find("committer")) == std::string::npos) 
			continue;
		jsondata.erase(0, pos);

		if ((pos = jsondata.find("\"name\"")) == std::string::npos) 
			continue;
		jsondata.erase(0, pos + 6);

		if ((start = jsondata.find("\"")) == std::string::npos)
			continue;
		if ((end = jsondata.find("\"", start + 1)) == std::string::npos)
			continue;

		std::string name = jsondata.substr(start + 1, end - start - 1);
		std::for_each(name.begin(), name.end(), [](char & c) { c = ::tolower(c); });

		// If the author name is valid, then return it
		if (name != "github") {
			authorname = name;
			break;
		}
	}

	
	return authorname;
}

