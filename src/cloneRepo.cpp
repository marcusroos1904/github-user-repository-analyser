
#include "definitions.h"

#include <iostream>

/* ------------------------------------------------------------------------------------------------
 * Clones the repository from the URL given as the parameter: repoUrl
 * The repository will be cloned into a directory with the named specified as the parameter: repoName
 * The cloned repository will be set to the "repo" parameter on success
 * 
 * Returns 0 on success and -1 on failure
 * ------------------------------------------------------------------------------------------------ */
int cloneRepo(const std::string& repoName, const std::string& repoUrl, git_repository* repo) 
{
    repo = NULL;
    std::string path = CLONED_REPOS_DIR + repoName;
    std::cout << "Cloning: '" << repoUrl << "' to: '" << path << "'" << std::endl;

    // Clone the given repo
    int error = git_clone(&repo, repoUrl.c_str(), path.c_str(), NULL);
    if (error < 0) {
        const git_error* e = git_error_last();
        fprintf(stderr, "Error %d/%d: %s\n", error, e->klass, e->message);
        return -1;
    }
    
    return 0;
}