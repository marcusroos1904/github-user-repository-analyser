
#include "definitions.h"
#include <vector>

/* ------------------------------------------------------------------------------------------------
 * Calculates and return the MRE score based on the data that was passed as the parameter: all_authors
 * 
 * Returns the MRE score on success and -1 on failure
 * ------------------------------------------------------------------------------------------------ */
int calculateMRE(const std::vector<AuthorData>& all_authors) 
{
    if (all_authors.empty()) {
        return -1;
    }

    int total = 0;            // The sum of all days between commits for all authors (that has made atleast 2 commits)
    int numberOfAuthors = 0;  // The number of authors that has made atleast 2 commits

    // Go through all the AuthorData in the set of authors
    for (int i = 0; i < all_authors.size(); i++) 
    {
        if (all_authors[i].commits < 2 || all_authors[i].firstCommit == 0 || all_authors[i].lastCommit == 0) {
            continue;  // Skip authors that has less than two commits
        }

        int diff = all_authors[i].lastCommit - all_authors[i].firstCommit;
        total += diff;
        numberOfAuthors++;
    }

    if (numberOfAuthors <= 0) {
        return 0;
    }

    // Calculate and return the MRE score
    int MRE = (int) (total / numberOfAuthors);
    return MRE;
}
