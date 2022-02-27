# README
  
To build the project, run make inside the directory  
  
In order for the project to build, you need to have libgit2 installed, and 2 other libraries.  
Here is a step by step guide that should work using Homebrew:  

brew install openssl  
brew install libssh2  
brew install libgit2 
  
Make sure libgit2 was installed inside: /usr/local/Cellar/libgit2/1.3.0  
If it was installed somwhere else, or if the version number is different, then you should change the directory path inside the Makefile  

  
  
NOTE:  
We might need some other way to include libgit2 when we submit this project, in case this doesn't work for the teacer
