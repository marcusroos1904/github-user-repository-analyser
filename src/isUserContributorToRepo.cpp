
#include "definitions.h"
#include <vector>
#include <string>
#include <algorithm>
#include <iostream>

/* ------------------------------------------------------------------------------------------------
 * Checks if the given username is actually one of the contributers for the repository
 * The reposiotry is represented as the paremter: all_authors, which contains 
 * all AuthorData objects from the repository, including all the names of the contributers
 * 
 * Returns true or false
 * ------------------------------------------------------------------------------------------------ */
bool isUserContributorToRepo(const std::string& username, const std::vector<AuthorData>& all_authors)
{
	std::string name = username;
	bool match = false;

	for (int i = 0; i < all_authors.size(); i++) {
	    // Compare the names in lower case
	    std::string currentName(all_authors[i].name);
	    std::for_each(currentName.begin(), currentName.end(), [](char & c) { c = ::tolower(c); });
	    std::for_each(name.begin(), name.end(), [](char & c) { c = ::tolower(c); });
	    if (currentName == name) {
	        match = true;
	        break;
	    }
	}
	
	return match;
}