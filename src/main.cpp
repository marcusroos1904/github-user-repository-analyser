
#include "definitions.h"
#include <git2.h>
#include <iostream>
#include <vector>
#include <cstring>
#include <limits>
#include <algorithm>
#include <curl/curl.h>
#include <curl/easy.h>

void before_exit(const std::string& msg)
{
    std::cerr << msg << std::endl;
    std::string rm_command = "rm -rf ";
    rm_command += CLONED_REPOS_DIR;
    system(rm_command.c_str()); 
}

size_t write_data(void *buffer, size_t size, size_t nmemb, void *userp)
{
    return size * nmemb;
}

int check_url(char *url)
{
    CURL *curl;
    CURLcode response;
    curl = curl_easy_init();
    long ct;
    if(curl) {
        std::cout.setstate(std::ios::failbit);
        curl_easy_setopt(curl, CURLOPT_URL, url);
        curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, write_data);
        response = curl_easy_perform(curl);
        if (CURLE_OK == response){
            response = curl_easy_getinfo(curl, CURLINFO_RESPONSE_CODE, &ct);
        }
    }
    curl_easy_cleanup(curl);
    return (ct != 200) ? 0 : 1;
}

int menu() {
        std::cout << "===== Github User Repository Analyser =====" << std::endl;
        std::cout << "1. Enter Github Username" << std::endl;
        std::cout << "2. Load Github Username list (Might not be added)." << std::endl;
        std::cout << "0. Exit" << std::endl;
        std::cout << "===========================================" << std::endl;
        std::cout << "Enter choice" << std::endl;
        int choice = 0;
        std::cin >> choice;
        while (std::cin.fail() || choice < 0 || choice > 2) {
            std::cout << "Wrong input.\n";
            std::cin.clear();
            std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');
            std::cin >> choice;
        }
    return choice;
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

    int choice = menu();
    std::string username;
    switch(choice)
    {
        case 0:
        default:
            return 0;
        case 1:
            std::cout << "Enter Github Username: \n";
            std::cin >> username;
            // Call function to handle one username
            break;
        case 2:
            std::cout << "Load Github Username list: \n";
            std::cin >> username;
            // Call function to handle multiple usernames
            break;
    }
    std::string userUrl = "https://github.com/" + username;
    char* char_arr;
    char_arr = &userUrl[0];
    std::cout << "UserUrl: " << userUrl << std::endl << std::endl;

    int result = check_url(char_arr);
    if (result)
        printf("User exists!\n");
    else
        printf("User does not exist!\n");
   
    // Get a vector of url's for all the users repository 
    std::vector<std::string> allRepos = getAllRepoUrls(userUrl);  // TODO: Implement this function for real (right now it returns a hard coded test repo)

    

    // Loop through all the repos
    std::vector<RepoData> all_repo_data;
    for (int i = 0; i < allRepos.size(); i++)
    {
        // TODO: Create and implement this function for read
        // std::string repoName = getRepoNameFromUrl(allRepos[i]);  
        std::string repoName = "TESTNAME";  // This is just a hard coded test name 
        

        // Create the local path that will store the cloned repository
        std::string repoPath = CLONED_REPOS_DIR;
        repoPath += repoName;
        
        if (cloneRepo(repoPath, allRepos[i]) == -1) {
            std::cerr << "Failed to clone repo: " << allRepos[i] << std::endl;
            continue;  // TODO: Print all failed/skipped repos at the end of the summary
        }

        std::vector<AuthorData> all_authors = getAllAuthorData(repoPath);  
        if (all_authors.empty()) {
            before_exit("Failed to extract author data from repo: " + repoName);
            return 1;
        }

        
        //
        // TODO: The AUTHOR NAME is going to be set to the Git-username, but the "username" given as input is the GitHub username
        //       This will casue problems when looking for lambdas made by the given user, since we only have their GitHub username
        //       How should we fix this? No idea right now. A problem for another day
        //
        std::string TEST_NAME = "Marco Magdy"; // TODO: Replace this with the requested author name (see todo above for why we can't pass input/username)
        //std::string TEST_NAME = "MikeWallaceDev"; 

        

        int total_lambdas_by_user = getTotalLambdasByUser(repoPath, TEST_NAME); 
        if (total_lambdas_by_user == -1) {
            before_exit("Failed to find lambdas for repo: " + repoName);
            return 1;
        }

        int MRE_score = calculateMRE(all_authors);
        if (MRE_score == -1) {
            before_exit("Failed to calculate the MRE score for repo: " + repoName);
            return 1;
        }

        float URE_score = calculateURE(TEST_NAME, all_authors);
        if (URE_score == -1) {
            before_exit("Failed to calculate the URE score for repo: " + repoName);
            return 1;
        }

        int number_of_commits = getNumberOfCommits(all_authors, TEST_NAME);
        if (number_of_commits == -1) {
            before_exit("Failed to find the total number of commits for: " + username + " in repo: " + repoName);
            return 1;
        }

        float avg_lambdas_per_commit = (total_lambdas_by_user / (float)number_of_commits);



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
        // TODO: The URE should be printed as a percentage
        //

        // THIS IS TEMPORARY (Make it look better later)
        fprintf(stderr, "\nRepo: %s\nMRE: %d\nURE: %.2f\nLambdas: %d\nCommits: %d\nLambdas/commit: %.2f\n\n",
     repoData.name, repoData.MRE, repoData.URE, repoData.lambdas, number_of_commits, repoData.avg_lambdas_per_commit);
    
    }


    //
    // TODO: Print a summary for all the repos. The summary should ATLEAST contain: full RepoData for the repo that has the MIN, MAX, and MEAN values for the URE score
    //
    
    
    // Delete the folder containing all the cloned repos
    system(rm_command.c_str());

    return 0;
}

