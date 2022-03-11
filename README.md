# README

## About
  
This program is used to analyze C++ GitHub repositories from the perspective of a given GitHub user.  
  
It will calculate and output the following for each repository:  
 * MRE (The average time a contributor has spent working on the repository)
 * URE (Which percentile the given user falls into, for the time spent working on the repository)
 * Number of lambdas used by the given user
 * Number of commits made by the given user
 * Average lambdas per commit made by the given user  
  
These values can be used to research if more experienced developers (higher URE values)  
uses lambdas more than less experienced developers (lower URE values).  
When researching this, it will probably be a good idea to only look at more "competetive" repositories.  
That means repositories that are popular, has a lot of contributors, and/or a high MRE value.
  

## How to use the program  

The program will take a GitHub user as input, and all analyzing will be from the perspecitive of this user.  
Before starting the program, place all C++ repositories you want to be analyzed inside the file: PLACE_REPOS_HERE.txt  
Only include repositories that the given user has contributed to.  
  
PLACE_REPOS_HERE.txt  
Place the full URL's of the repositories here. One repository per line
```
https://github.com/randomusername/randomreponame
url2
url2
...

```


## How to build on Linux  
  
The program requires the libgit2 library  

ALWAYS update the OS first!  
sudo apt update  
sudo apt full-upgrade -y  
install cmake (apt install cmake)  
install openssl (apt install libssl-dev)  
install git (apt install git)  
install g++ (apt install g++)  
install libgit2 (apt install libgit2-dev)  
install curl for openssl (apt install libcurl4-openssl-dev)  
Download Github User Repository Analyser (git clone https://github.com/marcusroos1904/github-user-repository-analyser.git)  
Once finished > cd to github user repository analyser root > 'make'  
Run the program!  
  

## How to build on MacOS
  
To build the project, run make inside the directory  
  
In order for the project to build, you need to have libgit2 installed, and 2 other libraries.  
Here is a step by step guide that should work using Homebrew:  

brew install openssl  
brew install libssh2  
brew install libgit2 
  
Make sure libgit2 was installed inside: /usr/local/Cellar/libgit2/1.3.0  
If it was installed somwhere else, or if the version number is different, then you should change the directory paths inside the Makefile   

Once these libraries have been installed:  
Download Github User Repository Analyser (git clone https://github.com/marcusroos1904/github-user-repository-analyser.git)  
Once finished > cd to github user repository analyser root > 'make'  
Run the program!  
  


  
