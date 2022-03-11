
# Paths for MacOS
# libgit2 should have been installed using brew. You need to change the paths below if you use a different version than 1.3.0
libgit2_include = /usr/local/Cellar/libgit2/1.3.0/include
libgit2_lib     = /usr/local/Cellar/libgit2/1.3.0/lib

# Set CFLAGS depending on the OS
UNAME_S := $(shell uname -s)
ifeq ($(UNAME_S),Linux)
    CFLAGS = -O3 -std=c++11 -lssh2 -lgit2 -lcurl
endif
ifeq ($(UNAME_S),Darwin)
    CFLAGS = -O3 -std=c++11 -I$(libgit2_include) -L$(libgit2_lib) -framework CoreFoundation -framework Security -lcurl -lz -liconv -lssh2 -lgit2
endif

CC = g++
FILES = ./src/*.cpp
TARGET = GithubAnalyser

all:	
	@$(CC) $(FILES) $(CFLAGS) -o $(TARGET)

clean:	
	rm -rf $(TARGET)
