
#include "definitions.h"
#include <string.h>

/* ------------------------------------------------------------------------------------------------
 * Returns the total number of commits the user specified as the parameter: username has made
 * This data is extracted from the data specified as the parameter: all_authors
 * 
 * Returns the number of commits on success and -1 on failure
 * ------------------------------------------------------------------------------------------------ */
int getNumberOfCommits(const std::vector<AuthorData>& all_authors, const std::string& username) 
{
    // Loop through all authors and return the number of commits for the requested author
    for (int i = 0; i < all_authors.size(); i++) {
        if (strcmp(all_authors[i].name, username.c_str()) == 0) {
            return all_authors[i].commits;
        }    
    }

    return -1;
}
