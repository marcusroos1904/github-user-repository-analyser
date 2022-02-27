
#include <iostream>

int main() 
{
    // TODO: Add a good looking menu instead of this temporary one
    std::cout << "Input GitHub username for the user you want to analyze: " << std::endl;
    std::string username;
    std::cin >> username;
    std::cout << "Received: " << username << std::endl;

    
    // TODO: Validate if the user exist or not
    std::string userRepoUrl = "https://github.com/" + username;


    // TODO: Add feature to clone all the repositories for the given user
    // An idea is to clone the repos into a new folder that has the same name as the user
    // SO if the username is alex, then a new folder called alex will be created that contains all his repos
    /*
    if (cloneAllRepositories(username) == -1) {
        std::cout << "Failed to clone repositories for: " << username << std::endl;
        return 1;
    }
    */


    // TODO: Loop though all repos and calculate MRE for each of them, and URE for the user
    // Get list of all contributors
    // For each contributor, get the days between first and lastest commit
    // Calculate MRE (average days between first and last commit for all contributors)
    // Calculate URE for the given user (whice percentile the user falls in for all contributors)
    // If the current contributor from above is the user, then save the value seperatly
    // Use the users value to calculate which percentile he is in (URE)


    // TODO: Loop though all repos and count all lambdas in them


    // TODO: Loop though all repos and go through all commits that the given user has made
    // Count the number of commits
    // Count the number of lambas the user has used in all their commits
    // Calculate the average lambdas per commit





    // TODO: Print out the analyzed results as they gets calculated, and then print a summary at the end
    // Each analyzed repo should contain:
    //  * Repositories name
    //  * MRE score
    //  * URE score
    //  * Total lambdas used by user
    //  * Average lambdas per commit by user
    //
    // The summary should print the repos that has the MIN, MAX and MEAN values for the URE score

    return 0;
}
