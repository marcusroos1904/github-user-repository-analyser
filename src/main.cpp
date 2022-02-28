
#include "definitions.h"
#include <git2.h>
#include <iostream>
#include <vector>
#include <cstring>


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




    //
    // TODO: Add a good looking menu instead of this temporary one (this one doesn't even work lol)
    //
    std::cout << "(THIS DOES NOTHING RIGHT NOW) Input GitHub username for the user you want to analyze: " << std::endl;
    std::string username;
    std::cin >> username;
    std::cout << "Received: " << username << std::endl << std::endl;

   
    std::string userUrl = "https://github.com/" + username;
    
    //
    // TODO: Validate if the user exist or not
    //
   
    // Get a vector of url's for all the users repository 
    std::vector<std::string> allRepos = getAllRepoUrls(userUrl);  // TODO: Implement this function for read (right now it returns a hard coded test repo)



    // A vector to store all the relevant repo data for all the users repositories
    std::vector<RepoData> all_repo_data;

    // Loop through all the repos
    for (int i = 0; i < allRepos.size(); i++)
    {
        git_repository* repo = NULL;
        std::string repoName = "TESTNAME";  // This is just a hard coded test name 
        // std::string repoName = getRepoNameFromUrl(allRepos[i]);  // TODO: Create and implement this function for read

        // Clone the current repository
        if (cloneRepo(repoName, allRepos[i], repo) == -1) {
            std::cerr << "Failed to clone repo: " << allRepos[i] << std::endl;
            if (repo) {
                git_repository_free(repo);
            }
            // Skip this repo and move on to the next (TODO: Maybe print all failed/skipped repos at the end of the summary)
            continue;  
        }

        
        // 
        // TODO: A potential problem might be that the "username" might be different than the "author name" for the given commits. Investigate this!
        //


        //
        // TODO: Implement this function for real (returns -1 for now)
        //
        // Get the total number of lambdas used by the given user 
        int total_lambdas_by_user = getTotalLambdasByUser(repo, username); 
        if (total_lambdas_by_user == -1) {
            std::cerr << "Failed to get the numbers of lambdas for the given user in repo: " << repoName << std::endl;
            system(rm_command.c_str());  // Delete the folder containing all the cloned repos
            return 1;
        }


        //
        // TODO: Implement this function for real (returns an empty vector for now)
        //
        // Get a set of all authors for this repository. Each author will have all relevant data fields needed for later
        std::vector<AuthorData> all_authors = getAllAuthorData(repo);  
        if (all_authors.empty()) {
            std::cerr << "Failed to find any authors/contributers for repo: " << repoName << std::endl;
            system(rm_command.c_str());  // Delete the folder containing all the cloned repos
            return 1;
        }

        //
        // TODO: Implement this function for real (returns -1 for now)
        //
        // Calculate the MRE score for this repo
        int MRE_score = calculateMRE(all_authors);
        if (MRE_score == -1) {
            std::cerr << "Failed to calculate the MRE score for repo: " << repoName << std::endl;
            system(rm_command.c_str());  // Delete the folder containing all the cloned repos
            return 1;
        }


        //
        // TODO: Implement this function for real (returns -1 for now)
        //
        // Calculate the URE score for the given user for this repo
        float URE_score = calculateURE(MRE_score, all_authors);
        if (URE_score == -1) {
            std::cerr << "Failed to calculate the URE score for the given user in repo: " << repoName << std::endl;
            system(rm_command.c_str());  // Delete the folder containing all the cloned repos
            return 1;
        }


        //
        // TODO: Implement this function for real (returns -1 for now)
        //
        // Get the number of commits for the given user
        int number_of_commits = getNumberOfCommits(all_authors, username);
        if (number_of_commits == -1) {
            std::cerr << "Failed to find the total number of commits for: " << username << " in repo: " << repoName << std::endl;
            system(rm_command.c_str());  // Delete the folder containing all the cloned repos
            return 1;
        }


        // Calculate the average lambdas per commit for the given user
        float avg_lambdas_per_commit = total_lambdas_by_user / number_of_commits;


        // Create a RepoData object and add it to the list
        RepoData repoData;

        strcpy(repoData.name, repoName.c_str());
        repoData.MRE = MRE_score;
        repoData.URE = URE_score;
        repoData.lambdas = total_lambdas_by_user;
        repoData.avg_lambdas_per_commit = avg_lambdas_per_commit;

        all_repo_data.push_back(repoData);    


        //
        // TODO: Print the current RepoData to the user so he/she can easily understand it
        //


        // Free the memory for the current repo
        if (repo) {
            git_repository_free(repo);
        }
    }


    //
    // TODO: Print a summary for all the repos. The summary should ATLEAST contain: full RepoData for the repo that has the MIN, MAX, and MEAN values for the URE score
    //
    
    
    // Delete the folder containing all the cloned repos
    system(rm_command.c_str());

    return 0;
}

