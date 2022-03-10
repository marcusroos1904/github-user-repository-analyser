
#include "definitions.h"
#include <git2.h>
#include <iostream>
#include <vector>
#include <cstring>
#include <limits>
#include <algorithm>
#include <curl/curl.h>
#include <curl/easy.h>

int menu();
void before_exit(const std::string& msg);
size_t write_data(void *buffer, size_t size, size_t nmemb, void *userp);
int check_url(const char *url);


/* ------------------------------------------------------------------------------------------------
 * MAIN 
 * ------------------------------------------------------------------------------------------------ */
int main() 
{
    // Initiate the libgit2 library
    git_libgit2_init();

    // Create the folder that will hold all the cloned repos
    std::string rm_command = "rm -rf ";
    std::string mkdir_command = "mkdir ";
    rm_command += CLONED_REPOS_DIR;
    mkdir_command += CLONED_REPOS_DIR;
    system(rm_command.c_str());
    system(mkdir_command.c_str());


    // Show the menu and get the user input
    int choice = menu();
    std::string username;
    switch(choice) {
        case 0:
        default:
            return 0;
        case 1:
            std::cout << "Enter Github Username: " << std::endl;
            std::cin >> username;
            std::cin.clear();
            std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');
            break;
        case 2:
            std::cout << "Load Github Username list: \n";
            std::cin >> username;
            break;
    }
    
    // Check if the given user exists
    std::string userUrl = "https://github.com/" + username;
    if (!check_url(userUrl.c_str())) {
        before_exit(username + " is not a GitHub user!");
        return 1;
    }


    //
    // TODO: Implement this function for real, right now it's hard coded urls
    //
    std::vector<std::string> allRepos = getAllRepoUrls(userUrl);
    std::vector<RepoData> all_repo_data;
    int failedRepos = 0;

    // Loop though all the given repositories
    for (int i = 0; i < allRepos.size(); i++)
    {
        // Create the local path that will store the cloned repository
        std::string repoName = getRepoNameFromUrl(allRepos[i]);  
        std::string repoPath = CLONED_REPOS_DIR;
        repoPath += repoName;
    
        std::cout << std::endl << "Cloning repository (" << (i+1) << "/" << (allRepos.size()) << "): " << allRepos[i] << " ..." << std::endl;
        if (cloneRepo(repoPath, allRepos[i]) == -1) {
            std::cerr << "ERROR! Failed to clone repo: " << allRepos[i] << std::endl;
            failedRepos++;
            continue;
        }

        std::vector<AuthorData> all_authors = getAllAuthorData(repoPath);  
        if (all_authors.empty()) {
            std::cerr << "ERROR! Failed to extract author data from repo: " << repoName << std::endl;
            failedRepos++;
            continue;
        }
        
        std::string authorname = getAuthorname(username, allRepos[i]);
        if (authorname.empty()) {
            std::cerr << "ERROR! Failed to get the commit author name for the given GitHub user: '" << username << "'" << std::endl;
            failedRepos++;
            continue;
        }

        if (!isUserContributorToRepo(authorname, all_authors)) {
            std::cerr << "ERROR! The user: '" << authorname << "' is not a contributer in the repo: '" << repoName << "'" << std::endl;
            failedRepos++;
            continue;
        }        

        int total_lambdas_by_user = getTotalLambdasByUser(repoPath, authorname); 
        if (total_lambdas_by_user == -1) {
            std::cerr << "ERROR! Failed to find lambdas for repo: " << repoName << std::endl;
            failedRepos++;
            continue;
        }

        int MRE_score = calculateMRE(all_authors);
        if (MRE_score == -1) {
            std::cerr << "ERROR! Failed to calculate the MRE score for repo: " << repoName << std::endl;
            failedRepos++;
            continue;
        }

        float URE_score = calculateURE(authorname, all_authors);
        if (URE_score == -1) {
            std::cerr << "ERROR! Failed to calculate the URE score for repo: " << repoName << std::endl;
            failedRepos++;
            continue;
        }

        int number_of_commits = getNumberOfCommits(all_authors, authorname);
        if (number_of_commits == -1) {
            std::cerr << "ERROR! Failed to find the total number of commits for: " << username << ", in repo: " << repoName << std::endl;
            failedRepos++;
            continue;
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
        // TODO: username/authorname should also be printed
        //

        // THIS IS TEMPORARY (Make it look better later)
        fprintf(stderr, "\nUsername: %s\nRepo: %s\nMRE: %d\nURE: %.2f\nLambdas: %d\nCommits: %d\nLambdas/commit: %.2f\n\n",
     username.c_str(), repoData.name, repoData.MRE, repoData.URE, repoData.lambdas, number_of_commits, repoData.avg_lambdas_per_commit);
    
    }


    //
    // TODO: Print a summary for all the repos. The summary should ATLEAST contain: full RepoData for the repo that has the MIN, MAX, and MEAN values for the URE score
    // TODO: Print repos that failed to clone
    //
    
    
    // Delete the folder containing all the cloned repos
    system(rm_command.c_str());

    return 0;
}



/* ------------------------------------------------------------------------------------
 * Prints the main menu and returns the input from the user
 * ---------------------------------------------------------------------------------- */
int menu() 
{
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


/* ------------------------------------------------------------------------------------
 * Prints the given error message, and performs all actions 
 * that needs to happen before the program terminates
 * ---------------------------------------------------------------------------------- */
void before_exit(const std::string& msg)
{
    std::cerr << msg << std::endl;
    std::string rm_command = "rm -rf ";
    rm_command += CLONED_REPOS_DIR;
    system(rm_command.c_str()); 
}


/* ------------------------------------------------------------------------------------
 * These two functions are used to check if a given GitHub user exist or not
 * ---------------------------------------------------------------------------------- */
size_t write_data(void *buffer, size_t size, size_t nmemb, void *userp)
{
    return size * nmemb;
}

int check_url(const char *url)
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
    std::cout.clear();
    return (ct != 200) ? 0 : 1;
}



