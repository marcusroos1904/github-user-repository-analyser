
//
// This file contains:
// Data structures, and function definitions for the major functions used in the over all program flow
//

#pragma once

#include <git2.h>
#include <vector>
#include <string>

#define CLONED_REPOS_DIR "./repos/"


struct RepoData {
    char name[256];
    int MRE = 0;
    float URE = 0.0f;
    int lambdas = 0;
    float avg_lambdas_per_commit = 0.0f;
};

struct AuthorData {
    char name[256];
    int firstCommit = 0;
    int lastCommit = 0;
    int commits = 0;
};

std::vector<std::string> getAllRepoUrls(const std::string& url);
int cloneRepo(const std::string& repoName, const std::string& repoUrl, git_repository* repo);
int getTotalLambdasByUser(git_repository* repo, const std::string& username);
std::vector<AuthorData> getAllAuthorData(git_repository* repo);
int calculateMRE(const std::vector<AuthorData>& all_authors);
float calculateURE(int MRE_score, const std::vector<AuthorData>& all_authors);
int getNumberOfCommits(const std::vector<AuthorData>& all_authors, const std::string& username);

