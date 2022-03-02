
#include "definitions.h"
#include <git2.h>
#include <iostream>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <vector>

/* ------------------------------------------------------------------------------------------------
 * Returns a vector to AuthorData for the repo given as the parameter: repoPath
 * The vector represents a set of all authors for the given repo, and the struct contains all relevant data we need
 * 
 * Returns an empty vector on failure
 * ------------------------------------------------------------------------------------------------ */
std::vector<AuthorData> getAllAuthorData(const std::string& repoPath) 
{
    // Vector to hold the set of authors that will be returned on success
    std::vector<AuthorData> all_author_data;

    std::string path = repoPath;
    path += "/.git";
    
    // Open the given repository
    git_repository* repo = NULL;
    if (git_repository_open(&repo, path.c_str()) < 0) {
        std::cerr << "Failed to open repository: " << path << std::endl;
        return {};
    }

    // Try both master and main when reading HEAD
    std::string path_master = path;
    path_master += "/refs/heads/master";
    std::string path_main = path;
    path_main += "/refs/heads/main";

    // Open the file for HEAD
    FILE* head_fileptr;
    if ((head_fileptr = fopen(path_master.c_str(), "r")) == NULL) {
        if ((head_fileptr = fopen(path_main.c_str(), "r")) == NULL) {
            std::cerr << "Failed to read HEAD. Error when opening file, tried both: " << path_master << ", and: " << path_main << std::endl;
            return {};
        }
        path_master = path_main;
    }

    // Read the content of the file
    char head_rev[41];
    if (fread(head_rev, 40, 1, head_fileptr) != 1) {
        std::cerr << "Failed to read HEAD. Error when reading from: " << path_master << std::endl;
        fclose(head_fileptr);
        return {};
    }

    fclose(head_fileptr);



    // Create a git_oid object
    git_oid oid;
    if(git_oid_fromstr(&oid, head_rev) < 0) {
        fprintf(stderr, "Invalid git object: '%s'\n", head_rev);
        return {};
    }

    // Create a "walker" that will be used to go though all the commits
    git_revwalk* walker = NULL;
    git_revwalk_new(&walker, repo);
    git_revwalk_sorting(walker, GIT_SORT_TOPOLOGICAL);
    git_revwalk_push(walker, &oid);

    git_commit* commit;
    const git_signature* commit_signature;
    
    int counter = 0;

    // Loop though all the commits in the current repo
    while (git_revwalk_next(&oid, walker) != GIT_ITEROVER) 
    {
        counter++;

        // Lookup the next commit
        int error = git_commit_lookup(&commit, repo, &oid);
        if (error < 0) {
            const git_error *e = git_error_last();
            fprintf(stderr, "Failed to loopkup the next commit. Error %d/%d: %s\n", error, e->klass, e->message);
            git_revwalk_free(walker);
            return {};
        }

        // Get the signature data for the current commit
        commit_signature = git_commit_committer(commit);
        
        
        // Ignore the author if it's GitHub. This author is used to perform all the merged pull requests
        if (strcmp(commit_signature->name, "GitHub") == 0) {
            git_commit_free(commit);
            continue;
        }
        
        // Get the time of the commit as time since epoch (in seconds)
        git_time_t time_sec = commit_signature->when.time;
        int days_offset = time_sec / 86400;


        // Go through all authors in the set and check if the current author already exist
        bool doesAuthorAlreadyExist = false;
        int i = 0;
        while (i < all_author_data.size()) {
            if (strcmp(all_author_data[i].name, commit_signature->name) == 0) {
                doesAuthorAlreadyExist = true;
                break;
            }
            i++;
        }

        if (!doesAuthorAlreadyExist || all_author_data.empty())
        {
            // The current author does not already exist, so create a new one and add it to the set
            AuthorData data;
            strcpy(data.name, commit_signature->name);
            data.firstCommit = days_offset;
            data.lastCommit = 0;
            data.commits += 1;
            all_author_data.push_back(data);
        }
        else
        {
            // The current author already exist, so update the fields that needs to be updated
            all_author_data[i].commits += 1;

            // Update the firstCommit and lastCommit fields for the current AuthorData if needed
            if (days_offset < all_author_data[i].firstCommit) 
            {
                // Update the firstCommit with the new commit time
                int oldFirstCommit = all_author_data[i].firstCommit;
                all_author_data[i].firstCommit = days_offset;

                // Update lastCommit with the old value of firstCommit, if lastCommit was still set to 0
                if (all_author_data[i].lastCommit <= 0) {
                    all_author_data[i].lastCommit = oldFirstCommit;
                }
            }
            else if (days_offset > all_author_data[i].firstCommit && days_offset > all_author_data[i].lastCommit)
            {
                // Update the lastCommit with the new commit time
                all_author_data[i].lastCommit = days_offset;
            }
        }

        git_commit_free(commit);
    }

    git_revwalk_free(walker);
    
   
    // TODO: Remove this later!
    fprintf(stderr, "\ngetAllAuthorData finished!! (This can be removed later, this is just to see if all authors was found)\n");
    fprintf(stderr, "Number of commits: %d, Number of authors: %d\n", counter, (int)all_author_data.size()); 
    for (int i = 0; i < all_author_data.size(); i++) {
        fprintf(stderr, "Commits: %d, FirstCommit: %d, LastCommit: %d, Name: %s\n", all_author_data[i].commits, all_author_data[i].firstCommit, all_author_data[i].lastCommit, all_author_data[i].name);
    }
    fprintf(stderr, "\n");

    return all_author_data;
}




