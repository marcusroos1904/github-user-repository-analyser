
#include "definitions.h"

/* ------------------------------------------------------------------------------------------------
 * Calculates and return the URE score based on all the AuthorData that was passed as the parameter: all_authors
 * The URE score is which percentile the specified author falls into, for the days between first and last commits 
 * for all the authors in: all_authors
 * The given author is specified as the parameter: author_name
 * 
 * Returns the URE score on success and -1 on failure
 * ------------------------------------------------------------------------------------------------ */
float calculateURE(const std::string& author_name, const std::vector<AuthorData>& all_authors) 
{
    if (all_authors.empty()) {
        return -1.0f;
    }

    // This vector holds the days between each authors first and last commit (for those who has at least 2 commits)
    std::vector<int> daysBetweenCommits;

    // Will hold the days between first and last commit for the requested author
    int author_value = -1;  

    // Go through all the AuthorData in the set of authors
    for (int i = 0; i < all_authors.size(); i++) 
    {
        if (all_authors[i].commits < 2 || all_authors[i].firstCommit == 0 || all_authors[i].lastCommit == 0) {
            continue;  // Skip authors that has less than two commits
        }

        int diff = all_authors[i].lastCommit - all_authors[i].firstCommit;
        daysBetweenCommits.push_back(diff);

        // Check if the current author is the requested author
        if (strcmp(all_authors[i].name, author_name.c_str()) == 0) {
            author_value = diff;
        }
    }

    if (daysBetweenCommits.empty() || author_value == -1) {
        return -1.0f;
    }

    // Sort the vector from lowest to highest
    sort(daysBetweenCommits.begin(), daysBetweenCommits.end());
    
    // Find which position in the sorted vector belongs to the requested author (index + 1) 
    int position = -1;
    for (int i = 0; i < daysBetweenCommits.size(); i++) {
        if (author_value == daysBetweenCommits[i]) {
            position = (i + 1);
            break;
        } 
    }

    if (position == -1) {
        return -1;
    }

    float percentile = (float) position / daysBetweenCommits.size();

    // TODO: This is for debug info and can be removed later!
    fprintf(stderr, "\nURE score: %.2f\n", percentile);

    return percentile;
}
