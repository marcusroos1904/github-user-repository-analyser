

# Paths for MacOS
libgit2_include = /usr/local/Cellar/libgit2/1.3.0/include
libgit2_lib     = /usr/local/Cellar/libgit2/1.3.0/lib

# Flags for MacOS
CFLAGS = -I$(libgit2_include) -L$(libgit2_lib) -framework CoreFoundation -framework Security -lcurl -lz -liconv -lssh2 -lgit2

# Flags for Linux
# CFLAGS = -lssh2 -lgit2


CC = g++
FILES = main.cpp
TARGET = out

all:
	$(CC) $(CFLAGS) $(FILES) -o $(TARGET)

clean:
	rm -rf $(TARGET)

