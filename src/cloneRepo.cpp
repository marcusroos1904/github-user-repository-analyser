
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
    if (git_clone(&repo, repoUrl.c_str(), repoPath.c_str(), NULL) < 0) {   
        if (repo)
            git_repository_free(repo);
        return -1;
    }
    
    if (repo)
        git_repository_free(repo);
    return 0;
}
