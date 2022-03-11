
//
// This file contains:
// Data structures, and function definitions for the major functions used in the over all program flow
//

#pragma once

#include <git2.h>
#include <vector>
#include <string>
#include <sys/types.h>
#include <sys/wait.h>
#include <algorithm>
#include <cstring>

#define PLACE_REPOS_HERE "PLACE_REPOS_HERE.txt"
#define CLONED_REPOS_DIR "./repos/"


struct RepoData {
    char name[256];
    int MRE = 0;
    float URE = 0.0f;
    int commits = 0;
    int lambdas = 0;
    float avg_lambdas_per_commit = 0.0f;
    int authors = 0;
};

struct AuthorData {
    char name[256];
    int firstCommit = 0;
    int lastCommit = 0;
    int commits = 0;
};

std::vector<std::string> getAllRepoUrls();
std::string getRepoNameFromUrl(const std::string& url);
int cloneRepo(const std::string& repoPath, const std::string& repoUrl);
std::vector<AuthorData> getAllAuthorData(const std::string& repoPath);
std::string getAuthorname(const std::string username, const std::string& repoUrl);
bool isUserContributorToRepo(const std::string& username, const std::vector<AuthorData>& all_authors);
int getTotalLambdasByUser(const std::string& repoPath, const std::string& authorName);
int calculateMRE(const std::vector<AuthorData>& all_authors);
float calculateURE(const std::string& author_name, const std::vector<AuthorData>& all_authors);
int getNumberOfCommits(const std::vector<AuthorData>& all_authors, const std::string& username);
std::vector<std::string> getValidFileExtensions();


