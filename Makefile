
# MARCUS PATHS
# libgit2_include = /home/linuxbrew/.linuxbrew/Cellar/libgit2/1.3.0/include
# libgit2_lib     = /home/linuxbrew/.linuxbrew/Cellar/libgit2/1.3.0/lib

# ALEX PATHS
libgit2_include = /usr/local/Cellar/libgit2/1.3.0/include
libgit2_lib     = /usr/local/Cellar/libgit2/1.3.0/lib

CC = g++
CFLAGS = -I$(libgit2_include) -L$(libgit2_lib) -lz -lgit2

# Full complile command for MacOS (incase something breaks in the future)
# CFLAGS = -I$(libgit2_include) -L$(libgit2_lib) -framework CoreFoundation -framework Security -lcurl -lz -liconv -lssh2 -lgit2

FILES = main.cpp
TARGET = out


all:
	$(CC) $(CFLAGS) $(FILES) -o $(TARGET)

clean:
	rm -rf $(TARGET)

