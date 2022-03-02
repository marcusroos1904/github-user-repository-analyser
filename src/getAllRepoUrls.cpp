
#include "definitions.h"

/* ------------------------------------------------------------------------------------------------
 * Returns a vector of all repository URL's for the GitHub user given as the parameter: url
 * Returns an empty vector on failure or in no repos was found
 * ------------------------------------------------------------------------------------------------ */
std::vector<std::string> getAllRepoUrls(const std::string& url) 
{
    std::vector<std::string> allRepos;

    // THIS IS JUST A TEMPORARY TEST CODE
    // Hardcoded test urls 
    allRepos.push_back("https://github.com/AppFlowy-IO/AppFlowy");
    //allRepos.push_back("https://github.com/marcusroos1904/github-user-repository-analyser");

    return allRepos;
}
