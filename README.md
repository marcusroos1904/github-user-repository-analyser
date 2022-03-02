# README

## How to build on Linux  
  
The program requires the libgit2 library  
  
Download the latest release: https://github.com/libgit2/libgit2/releases (At time of writing, 1.4.2)  
Create new dir at /home/lib (mkdir /home/libg)  
Download -> unzip to /home/lib/  
cd to root (contains AUTHORS, SECURITY, COPYING etc)  
In terminal: mkdir build  
In temrinal: cd build  
In temrinal: cmake ..  
In temrinal: cmake --build .  
Once finished > cd to github user repository analyser root > 'make'  
In temrinal: LD_LIBRARY_PATH=/home/lib/libgit2-1.4.2/build/  
In terminal: export LD_LIBRARY_PATH  
  
Good to go  
./out  
  

## How to build on MacOS
  
To build the project, run make inside the directory  
  
In order for the project to build, you need to have libgit2 installed, and 2 other libraries.  
Here is a step by step guide that should work using Homebrew:  

brew install openssl  
brew install libssh2  
brew install libgit2 
  
Make sure libgit2 was installed inside: /usr/local/Cellar/libgit2/1.3.0  
If it was installed somwhere else, or if the version number is different, then you should change the directory path inside the Makefile  

  
