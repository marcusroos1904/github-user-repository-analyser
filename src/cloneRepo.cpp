
#include "definitions.h"

#include <git2.h>
#include <iostream>

/* ------------------------------------------------------------------------------------------------
 * Clones the repository from the URL given as the parameter: repoUrl
 * The repository will be cloned into a directory with the path specified as the parameter: repoPath 
 * 
 * Returns 0 on success and -1 on failure
 * ------------------------------------------------------------------------------------------------ */
int cloneRepo(const std::string& repoPath, const std::string& repoUrl)
{
    git_repository* repo = NULL;
    fprintf(stderr, "Cloning: %s, to path: %s\n", repoUrl.c_str(), repoPath.c_str());

    // Clone the given repo
    int error = git_clone(&repo, repoUrl.c_str(), repoPath.c_str(), NULL);
    if (error < 0) {
        const git_error* e = git_error_last();
        fprintf(stderr, "Failed to clone repo. Error %d/%d: %s\n", error, e->klass, e->message);    
        if (repo)
            git_repository_free(repo);
        
        return -1;
    }
    
    if (repo)
        git_repository_free(repo);

    return 0;
}
