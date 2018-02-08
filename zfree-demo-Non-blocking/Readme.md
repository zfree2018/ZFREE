# Build Code
## Prerequisites
### libtins
[libtins official guide](http://libtins.github.io/download/)    
#### Short guide for installing this library
* ubuntu 16.04
* Install build requirements
```
sudo apt install libpcap-dev libssl-dev cmake
```
* Get source code
```
git clone https://github.com/mfontanini/libtins.git
```
* Compiling
``` 
cd libtins
mkdir build
cd build
cmake ../ -DLIBTINS_ENABLE_CXX11=1 
make
sudo make install
```
### google test
* Get source code
`git clone https://github.com/google/googletest.git`
* Compiling
```
cd googletest
export GTEST_TARGET=googletest
./travis.sh
cd build/googletest
sudo make install 
```

## Compiling Project
```
cd <project-folder>
mkdir build
cd build
cmake ../
```

## Usage
The compiled binary is placed in `build/bin` directory. 

### Unit tests
The tests binary is placed in `build/test` directory.  
`./runTests` for running test cases under google test framework.

# Structure of proxy 1 and proxy 2
```
                                                                                 +-------------------+
+---------------------------------------------------------+                      |      proxy2-1     |
|      proxy 1                                            |                      |                   |
|                                                         |                      |  +-------------+  |
|  +--------------+       +--------------+                |                      |  |   sniffer   |  |
|  |   sniffer    |   +---+ TLS client 1 +--------------------------------------->  |             |  |
|  |              |   |   +--------------+                |                      |  +------|------+  |
|  +------|-------+   |                                   |                      |         |         |
|         |           |   +--------------+                |                      |  +------v------+  |
|         |           +---+ TLS client 2 +--------------------+                  |  |  HashTable  |  |
|  +------v-------+   |   +--------------+                |   |                  |  |             |  |
|  |              +---+                                   |   |                  |  +------|------+  |
|  |  HashTable   |   |   +--------------+                |   |                  |         |         |
|  |              |   +---+ TLS client 3 +--------+       |   |                  |  +------v------+  |
|  +--------------+       +--------------+        |       |   |                  |  |  TLS Server |  |
|                                                 |       |   |                  |  |             |  |
|                              ......             |       |   |                  |  +-------------+  |
+---------------------------------------------------------+   |                  |                   |
                                                  |           |                  +-------------------+
sniffer: libtins                                  |           |
multithread: pthread                              |           |
TLS connection: openssl                           |           |                  +-------------------+
Hash: SHA256(crypto from openssl)                 |           |                  |      proxy2-2     |
                                                  |           +----------------> |                   |
                                                  |                              |  +-------------+  |
                                                  |                              |  |   sniffer   |  |
                                                  |                              |  |             |  |
                                                  |
                                                  |                                   ......
                                                  |
                                                  +------------------------------>
```


# components of sniffer
```
+-------------Sniffer-------------+
|                                 |
|                                 |
|   +------Configuration-------+  |
|   |                          |  |
|   |     Network Interface    |  |
|   |                          |  |
|   |     Packet type          |  |
|   |                          |  |
|   +------------|-------------+  |
|                |                |
|                |                |
|                |                |
|   +------------v-------------+  |
|   |                          |  |
|   |   Catch packets          |  |
|   |                          |  |
|   | Get packet payload(data) |  |
|   |                          |  |
|   +------------|-------------+  |
|                |                |
|                |                |
|   +------------v-------------+  |
|   |                          |  |
|   |                          |  |
|   |  SHA256(payload)         |  |
|   |                          |  |
|   |                          |  |
|   +--------------------------+  |
|                                 |
|                                 |
|                                 |
+---------------------------------+

```


# How to build

## Prerequisites
You have to install `libtins` first. Follow the instructions here
[libtins installation](http://libtins.github.io/download/)

## Build
`mkdir build` under project root directory. Then `cd build && cmake ../`.
It will create `Makefile` for this project. Then simply using `make` command.
target binaries are placed in `bin/` under `build` directory we made.



