
#include "definitions.h"
#include <git2.h>
#include <iostream>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <cstring>
#include <unistd.h>
#include <sys/types.h>
#include <fstream>
#include <regex>
#include <algorithm>

#define DIFF_FILE "commit_diff.txt"


/* ------------------------------------------------------------------------------------------------
 * This is a static function, and is only used inside this file
 * 
 * This function searches for, and counts lambdas for the given "git diff"-file
 * It only checks files that has a "valid" file extension, and also only 
 * counts lambdas that has been ADDED, according to the diff-file
 * 
 * Return the number of lambdas that was found (0 as default)
 * ------------------------------------------------------------------------------------------------ */
static int countLambdas(const std::string& file)
{
    std::regex lambda_regex(R"([\,\=\s\(\)]*[\,\=\s\t\(\)]+\[[a-zA-Z0-9\*\,_&\s=:<>]*\]\s*(\(|\{))", std::regex::optimize);
    std::vector<std::string> validFileExtensions = getValidFileExtensions();

    // Open the diff file and read line by line
    std::ifstream fromFile(file);
    std::string line;        
    bool isFileChunk = false;
    bool isInsideBlockComment = false;

    int lambda_counter = 0;   
    while (std::getline(fromFile, line))
    {
        // Check if the current line signals the start of a new file chunk. Set the variable to false, if that is the case
        if (line.size() > 3) {
            bool isAdd = (line[0] == '+' && line[1] == '+' && line[2] == '+');
            bool isSub = (line[0] == '-' && line[1] == '-' && line[2] == '-');
            if (isAdd || isSub) {
                isFileChunk = false;
                isInsideBlockComment = false;
            }
        }
        
        // If we are inside a valid file chunk right now, then look for lines where code has been added, and search them for lambdas
        if (isFileChunk) 
        {
            if (line.size() > 2 && line[0] == '+') {
                line.erase(0, 1);  // Remove the first '+' character
                
                // Remove all strings from the line
                size_t stringPos = 0;
                size_t endStringPos = 0;
                if ((stringPos = line.find_first_of("\"")) != std::string::npos) {
                    if ((endStringPos = line.find_last_of("\"")) != std::string::npos) {
                        line.erase(stringPos + 1, endStringPos - stringPos - 1);
                    }
                }
                stringPos = 0;
                
                // If we are inside a block comment and '*/' is found, then we are now outside the block comment
                if (isInsideBlockComment) { 
                    if (line.find("*/") == std::string::npos) { 
                        isInsideBlockComment = false;
                    }
                }
                // We are not inside a block comment right now
                else
                {
                    // Skip '/*' comments
                    if ((stringPos = line.find("/*")) != std::string::npos) {
                        // Check for lambda before the block comment    
                        std::string substring = line.substr(0, stringPos);
                        if (std::regex_search(substring, lambda_regex)) {
                            fprintf(stderr, "%s\n", line.c_str());  // TODO: Remove this!
                            lambda_counter++;
                        }
                        // If '*/' is not found on this line, that means the following lines will be inside a block comment
                        if (line.find("*/") == std::string::npos) {
                            isInsideBlockComment = true;
                        }
                    }
                    
                    // Skip '//' comments
                    else if ((stringPos = line.find("//")) != std::string::npos) {
                        // Check for lambda before the line comment    
                        std::string substring = line.substr(0, stringPos);
                        if (std::regex_search(substring, lambda_regex)) {
                            fprintf(stderr, "%s\n", line.c_str());  // TODO: Remove this!
                            lambda_counter++;
                        }
                    }

                    // No code comment was found, just check the line for lambda
                    else if (std::regex_search(line, lambda_regex)) {
                        fprintf(stderr, "%s\n", line.c_str());  // TODO: Remove this!
                        lambda_counter++;
                    }
                }
            }
        }

        // If we are not inside a valid file chunk right now, then check if the line signals that an addition to a valid file has occured
        if (!isFileChunk)
        {
            if (line.size() > 3 && line[0] == '+' && line[1] == '+' && line[2] == '+') {
                std::string file_extension = line.substr(line.find_last_of(".") + 1);
                for (int i = 0; i < validFileExtensions.size(); i++) {
                    if (validFileExtensions[i] == file_extension) {
                        isFileChunk = true;
                        isInsideBlockComment = false;
                        break;
                    }    
                }        
            }
        }
    }

    fromFile.close();
    return lambda_counter;
}



/* ------------------------------------------------------------------------------------------------
 * Returns the total number of lambdas for the user given as the parameter: authorName
 * The lambdas will be searched for inside the local repository located at the path, given as the parameter: repoPath 
 * 
 * Returns the number of lambdas on success and -1 on failure
 * ------------------------------------------------------------------------------------------------ */
int getTotalLambdasByUser(const std::string& repoPath, const std::string& authorName)
{   
    fprintf(stderr, "Searching repository for lambdas...\n");
    int lambda_counter = 0;

    // Create the DIFF_FILE that stores the output from the git diff command later
    std::string rm_command = "rm -f ";
    rm_command += DIFF_FILE;
    std::string touch_command = "touch ";
    touch_command += DIFF_FILE;
    system(rm_command.c_str());
    system(touch_command.c_str());


    
    // Open the given repository
    std::string path = repoPath;
    path += "/.git";
    
    git_repository* repo = NULL;
    if (git_repository_open(&repo, path.c_str()) < 0) {
        std::cerr << "Failed to open repository: " << path << std::endl;
        system(rm_command.c_str());
        return -1;
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
            git_repository_free(repo);
            system(rm_command.c_str());
            return -1;
        }
        path_master = path_main;
    }

    // Read the content of the file
    char head_rev[41];
    if (fread(head_rev, 40, 1, head_fileptr) != 1) {
        std::cerr << "Failed to read HEAD. Error when reading from: " << path_master << std::endl;
        fclose(head_fileptr);
        git_repository_free(repo);
        system(rm_command.c_str());
        return -1;    
    }

    fclose(head_fileptr);


    // Create a git_oid object
    git_oid oid;
    if (git_oid_fromstr(&oid, head_rev) < 0) {
        std::cerr << "Failed to create git object from the content that was read from HEAD" << std::endl;
        git_repository_free(repo);
        system(rm_command.c_str());
        return -1;
    }

    // Create a "walker" that will be used to go through all the commits
    git_revwalk* walker = NULL;
    git_revwalk_new(&walker, repo);
    git_revwalk_sorting(walker, GIT_SORT_TOPOLOGICAL | GIT_SORT_TIME);
    git_revwalk_push(walker, &oid);



    // Loop through all the commits in the current branch
    git_commit* commit = NULL;
    while (git_revwalk_next(&oid, walker) != GIT_ITEROVER) 
    {
        // Lookup the next commit
        if (git_commit_lookup(&commit, repo, &oid) < 0) {
            std::cerr << "Failed to lookup the next commit while checking repo: " << repoPath << std::endl;
            if (commit)
                git_commit_free(commit);
            git_revwalk_free(walker);
            git_repository_free(repo);
            system(rm_command.c_str());
            return -1;
        }
        
        // Convert the commit name to lower case
        const git_signature* commit_signature = git_commit_committer(commit);
        std::string name_lowercase(commit_signature->name);
        std::for_each(name_lowercase.begin(), name_lowercase.end(), [](char & c) { c = ::tolower(c); });

        // Only check the commit that are made by the requested author
        if (name_lowercase != authorName) {
            git_commit_free(commit);
            continue;
        }
        
        // Get the commit id and convert it to a hex string
        const git_oid* commit_id = git_commit_id(commit);
        std::string commit_hash;
        char hex_str[256];
        int i;
        for (i = 0; i < sizeof(commit_id->id); i++)
            sprintf((hex_str + i*2), "%02x", commit_id->id[i]);
        hex_str[i*2] = '\0';
        commit_hash.append(hex_str);



        // Construct the shell command for calling "git diff" on the current commit and its parent. The output is saved to the given DIFF_FILE
        std::string cmd = "git --git-dir=";
        cmd = cmd + repoPath + "/.git diff " + commit_hash + "^! > " + DIFF_FILE;

        // Fork a child process
        int pid = fork();
        if (pid == -1) {
            perror("Failed to fork a child process");
            git_commit_free(commit);
            git_revwalk_free(walker);
            git_repository_free(repo);
            system(rm_command.c_str());
            return -1;
        }

        // Execute the shell command in the child process
        if (pid == 0) {
            execl("/bin/sh", "sh", "-c", cmd.c_str(), 0);
            return 0;
        }

        while (wait(NULL) > 0) ;


        // Search the diff file and count the lambdas
        lambda_counter += countLambdas(DIFF_FILE);

        // Free the allocated memory for the current commit
        if (commit)
            git_commit_free(commit);
    } 


    git_revwalk_free(walker);
    git_repository_free(repo);
    system(rm_command.c_str());

    return lambda_counter;
}




