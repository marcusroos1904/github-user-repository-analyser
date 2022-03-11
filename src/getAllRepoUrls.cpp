
#include "definitions.h"

#include <iostream>
#include <vector>
#include <string>
#include <fstream>
#include <curl/curl.h>
#include <curl/easy.h>

static size_t write_data(void *buffer, size_t size, size_t nmemb, void *userp)
{
    return size * nmemb;
}

static int check_url(const char *url)
{
    CURL *curl;
    CURLcode response;
    curl = curl_easy_init();
    long ct;
    if(curl) {
        std::cout.setstate(std::ios::failbit);
        curl_easy_setopt(curl, CURLOPT_URL, url);
        curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, write_data);
        response = curl_easy_perform(curl);
        if (CURLE_OK == response){
            response = curl_easy_getinfo(curl, CURLINFO_RESPONSE_CODE, &ct);
        }
    }
    curl_easy_cleanup(curl);
    std::cout.clear();
    return (ct != 200) ? 0 : 1;
}

/* ------------------------------------------------------------------------------------------------
 * Returns a vector of all repository URL's that was placed inside the given file on success
 * Returns an empty vector on failure or in no repos was found
 * ------------------------------------------------------------------------------------------------ */
std::vector<std::string> getAllRepoUrls() 
{
    std::vector<std::string> allRepos;

    // Open the file
    std::ifstream fromFile(PLACE_REPOS_HERE);
    if (fromFile.fail()) {
        std::cerr << "ERROR! The file: " << PLACE_REPOS_HERE << " doesn't exist" << std::endl;
        return allRepos; 
    }

    // Read the file line by line
    std::string line;  
    while (std::getline(fromFile, line)) 
    {
        // Skip lines that have been commented out or are empty
        if ((!line.empty() && line[0] == '#') || line.empty()) {
            continue;
        }

        // Check if the given repo is a valid url
        if (check_url(line.c_str())) {
            allRepos.push_back(line);
        } 
    }

    return allRepos;
}



