# README

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
  


  
