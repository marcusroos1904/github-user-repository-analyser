
#include "definitions.h"
#include <git2.h>
#include <iostream>
#include <vector>
#include <cstring>
#include <limits>
#include <algorithm>
#include <curl/curl.h>
#include <curl/easy.h>
#include <functional>

int menu();
size_t write_data(void *buffer, size_t size, size_t nmemb, void *userp);
int check_url(const char *url);
void printRepoData(const RepoData& repoData);


/* ------------------------------------------------------------------------------------------------
 * MAIN 
 * ------------------------------------------------------------------------------------------------ */
int main() 
{
    // Load the repositories to analyze
    std::vector<std::string> allRepos = getAllRepoUrls();
    if (allRepos.empty()) {
        std::cerr << "ERROR! Place the GitHub repositories you want to analyze in: " << PLACE_REPOS_HERE << std::endl;
        std::cerr << "See the README file for more information on how to user the program" << std::endl;
        return 1;
    }
    std::cout << "\nLoaded " << allRepos.size() << " repositories from: " << PLACE_REPOS_HERE << std::endl;


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
    // The menu can be expanded to include more options in the future
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
    }
    

    // Check if the given user exists
    std::string userUrl = "https://github.com/" + username;
    if (!check_url(userUrl.c_str())) {
        std::cerr << "ERROR! " << username << " is not a GitHub user!" << std::endl;
        system(rm_command.c_str());
        return 1;
    }

    // Loop though all the given repositories
    std::vector<RepoData> all_repo_data;
    std::vector<std::string> failedRepos;
    for (int i = 0; i < allRepos.size(); i++)
    {
        // Create the local path that will store the cloned repository
        std::string repoName = getRepoNameFromUrl(allRepos[i]);  
        std::string repoPath = CLONED_REPOS_DIR;
        repoPath += repoName;
    
        std::cout << std::endl << "Cloning repository (" << (i+1) << "/" << (allRepos.size()) << "): " << allRepos[i] << " ..." << std::endl;
        if (cloneRepo(repoPath, allRepos[i]) == -1) {
            std::cerr << "ERROR! Failed to clone repo: " << allRepos[i] << std::endl;
            failedRepos.push_back(repoName);
            continue;
        }

        std::cout << "Extracting data from repository..." << std::endl;
        std::vector<AuthorData> all_authors = getAllAuthorData(repoPath);  
        if (all_authors.empty()) {
            std::cerr << "ERROR! Failed to extract author data from repo: " << repoName << std::endl;
            failedRepos.push_back(repoName);
            continue;
        }
        
        std::string authorname = getAuthorname(username, allRepos[i]);
        if (authorname.empty()) {
            std::cerr << "ERROR! Failed to find any matching author for the given GitHub user: '" << username << "'" << std::endl;
            failedRepos.push_back(repoName);
            continue;
        }

        if (!isUserContributorToRepo(authorname, all_authors)) {
            std::cerr << "ERROR! The user: '" << authorname << "' is not a contributer in the repo: '" << repoName << "'" << std::endl;
            failedRepos.push_back(repoName);
            continue;
        }        

        std::cout << "Searching repository for lambdas..." << std::endl;
        int total_lambdas_by_user = getTotalLambdasByUser(repoPath, authorname); 
        if (total_lambdas_by_user == -1) {
            std::cerr << "ERROR! Failed to find lambdas for repo: " << repoName << std::endl;
            failedRepos.push_back(repoName);
            continue;
        }

        int MRE_score = calculateMRE(all_authors);
        if (MRE_score == -1) {
            std::cerr << "ERROR! Failed to calculate the MRE score for repo: " << repoName << std::endl;
            failedRepos.push_back(repoName);
            continue;
        }

        float URE_score = calculateURE(authorname, all_authors);
        if (URE_score == -1) {
            std::cerr << "ERROR! Failed to calculate the URE score for repo: " << repoName << std::endl;
            failedRepos.push_back(repoName);
            continue;
        }

        int number_of_commits = getNumberOfCommits(all_authors, authorname);
        if (number_of_commits == -1) {
            std::cerr << "ERROR! Failed to find the total number of commits for: " << username << ", in repo: " << repoName << std::endl;
            failedRepos.push_back(repoName);
            continue;
        }

        float avg_lambdas_per_commit = (total_lambdas_by_user / (float)number_of_commits);


        // Create a RepoData object and add it to the list
        RepoData repoData;
        strcpy(repoData.name, repoName.c_str());
        repoData.authors = all_authors.size();
        repoData.MRE = MRE_score;
        repoData.URE = URE_score;
        repoData.commits = number_of_commits;
        repoData.lambdas = total_lambdas_by_user;
        repoData.avg_lambdas_per_commit = avg_lambdas_per_commit;
        all_repo_data.push_back(repoData);    

        // Print the analyzed data for the current repository
        std::cout << " * Repository (" << (i+1) << "/" << (allRepos.size()) << ")" << std::endl;
        printRepoData(repoData);
    }




    // Terminate the program if no repositories were analyzed
    if (all_repo_data.empty()) {
        std::cerr << "No repositories were analyzed successfully..." << std::endl;
        system(rm_command.c_str());
        return 1;
    }

    // Find the index of the MAX and MIN value for the URE scores
    float URE_MIN = 200.0f;
    float URE_MAX = -200.0f;
    int index_MIN = -1; 
    int index_MAX = -1;
    for (int i = 0; i < all_repo_data.size(); i++) {
        if (all_repo_data[i].URE < URE_MIN) {
            URE_MIN = all_repo_data[i].URE;
            index_MIN = i;
        }
        if (all_repo_data[i].URE > URE_MAX) {
            URE_MAX = all_repo_data[i].URE;
            index_MAX = i;
        }
    }

    // Find the MEAN repository for their URE scores
    std::vector<float> ure_values;
    for (int i = 0; i < all_repo_data.size(); i++)
        ure_values.push_back(all_repo_data[i].URE);
    std::sort(ure_values.begin(), ure_values.end());
    
    float URE_MEAN = ure_values[(int)(ure_values.size() / 2)];
    int index_MEAN = -1;
    for (int i = 0; i < all_repo_data.size(); i++) {
        if (all_repo_data[i].URE == URE_MEAN) {
            index_MEAN = i;
            break;
        }
    }


    // Print a summary
    std::cout << std::endl << "----------------- SUMMARY -----------------" << std::endl;
    std::cout << "Analyzed " << all_repo_data.size() << " repositories for " << username << std::endl;

    // Print the MIN, MAX and MEAN repositories for their URE scores
    if (index_MIN >= 0) {
        std::cout << std::endl << " * Repository with lowest URE" << std::endl;
        printRepoData(all_repo_data[index_MIN]);
    }
    if (index_MAX >= 0) {
        std::cout << std::endl << " * Repository with highest URE" << std::endl;
        printRepoData(all_repo_data[index_MAX]);
    }
    if (index_MEAN >= 0) {
        std::cout << std::endl << " * Repository with mean URE" << std::endl;
        printRepoData(all_repo_data[index_MEAN]);
    }

    // List all repositories that failed to be analyzed
    if (!failedRepos.empty()) {
        std::cerr << "\nFailed to analyze " << failedRepos.size() << " repositories" << std::endl;
        for (int i = 0; i < failedRepos.size(); i++) {
            std::cerr << failedRepos[i] << std::endl;
        }
    }   

    std::cout << std::endl;
    
    // Delete the folder containing all the cloned repos
    system(rm_command.c_str());

    return 0;
}



/* ------------------------------------------------------------------------------------
 * Prints the main menu and returns the input from the user
 * ---------------------------------------------------------------------------------- */
int menu() 
{
    std::cout << "\n========== Github User Repository Analyser ==========" << std::endl;
    std::cout << "1. Enter Github Username" << std::endl;
    std::cout << "0. Exit" << std::endl;
    std::cout << "=====================================================" << std::endl;
    std::cout << "Enter choice" << std::endl;
    int choice = 0;
    std::cin >> choice;
    while (std::cin.fail() || choice < 0 || choice > 1) {
        std::cout << "Wrong input.\n";
        std::cin.clear();
        std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');
        std::cin >> choice;
    }
    return choice;
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


/* ------------------------------------------------------------------------------------
 * This function prints out the given repository data
 * ---------------------------------------------------------------------------------- */
void printRepoData(const RepoData& repoData)
{
    int percentile = (repoData.URE * 100);
    std::cout << "    Repository:     " << repoData.name << std::endl;
    std::cout << "    MRE:            " << repoData.MRE << std::endl;
    std::cout << "    URE:            " << percentile << "%" << std::endl;
    std::cout << "    Contributors:   " << repoData.authors << std::endl;
    std::cout << "    Commits:        " << repoData.commits << std::endl;
    std::cout << "    Lambdas:        " << repoData.lambdas << std::endl;
    std::cout << "    Lambdas/commit: " << repoData.avg_lambdas_per_commit << std::endl;
}




