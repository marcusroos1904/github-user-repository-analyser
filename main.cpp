
#include <iostream>
#include <vector>
#include <git2.h>


#define CLONED_REPOS_DIR "./repos/"


/* ------------------------------------------------------------------------------------------------
 * Returns a vector of all repo URL's for the given GitHub user 
 * Returns an empty vector on failure or in no repos was found
 * ------------------------------------------------------------------------------------------------ */
std::vector<std::string> getAllRepoUrls(const std::string& url) 
{
    // THIS IS JUST A TEMPORARY TEST CODE
    std::vector<std::string> allRepos;
    allRepos.push_back("https://github.com/AppFlowy-IO/AppFlowy");
    //allRepos.push_back("https://github.com/marcusroos1904/github-user-repository-analyser");

    return allRepos;
}


/* ------------------------------------------------------------------------------------------------
 * Clones the given repository. 
 * The repository will be set to the "repo" parameter on success
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


/* ------------------------------------------------------------------------------------------------
 * MAIN 
 * ------------------------------------------------------------------------------------------------ */
int main() 
{
    // Initiate the libgit2 library
    git_libgit2_init();
    
    // Clears the folder that holds all the cloned repos
    std::string rm_command = "rm -rf ";
    std::string mkdir_command = "mkdir ";
    rm_command += CLONED_REPOS_DIR;
    mkdir_command += CLONED_REPOS_DIR;
    
    system(rm_command.c_str());
    system(mkdir_command.c_str());



    // TODO: Add a good looking menu instead of this temporary one
    std::cout << "(THIS DOES NOTHING RIGHT NOW) Input GitHub username for the user you want to analyze: " << std::endl;
    std::string username;
    std::cin >> username;
    std::cout << "Received: " << username << std::endl << std::endl;

   


    // TODO: Validate if the user exist or not
    std::string userUrl = "https://github.com/" + username;
    
    // TODO: Get the list of all repos (URL's) for the given user
    std::vector<std::string> allRepos = getAllRepoUrls(userUrl); 


    // Loop through all the repos
    for (int i = 0; i < allRepos.size(); i++)
    {
        git_repository* repo = NULL;
        std::string repoName = "TESTNAME";  // getRepoNameFromUrl(allRepos[i]);  TODO: Create a function that gets the repo name from an URL
        
        // Clone the current repository
        if (cloneRepo(repoName, allRepos[i], repo) == -1) {
            std::cerr << "Failed to clone repo: " << allRepos[i] << std::endl;
            if (repo) {
                git_repository_free(repo);
            }
            continue;
        }

        std::cout << "CLONED REPO!!! YAY :D" << std::endl;
        
        
        //
        // TODO: Go through the codebase and count all the lambdas that has been added by the requested user
        //
        // TODO: Go through all the commits and create a set of all contributers. Each contributer should be represented as a struct and contains the following data:
        //  * Username
        //  * Date of first commit
        //  * Date of last commit
        //  * Total number of commits
        //
        // TODO: Based on this set calculate the MRE and URE scores
        //
        // TODO: Create a struct that can hold all the data we need to extract from each repo. The struct should contain:
        //  * Repositories name
        //  * MRE score
        //  * URE score
        //  * Total lambdas used by user
        //  * Average lambdas per commit by user
        //


        // Free the memory for the current repo
        if (repo) {
            git_repository_free(repo);
        }
    }

    
    
    // TODO: Print out the analyzed results as they gets calculated, and then print a summary at the end
    // Each analyzed repo should contain:
    //  * Repositories name
    //  * MRE score
    //  * URE score
    //  * Total lambdas used by user
    //  * Average lambdas per commit by user
    //
    // The summary should print the repos that has the MIN, MAX and MEAN values for the URE score


    
    // Delete the folder containing all the cloned repos
    system(rm_command.c_str());

    return 0;
}
