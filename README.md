# CCHAT (Segfault Chat)

A simple twitch chat reader for you terminal.

## Table of contents (optional)

- Requirements
- Clone & Compiling
- Config & Usage


## Requirements

To compile this abomination you will need the following:

- [gmake](https://www.gnu.org/software/make/)
- [libconfig](http://hyperrealm.github.io/libconfig/)
- GCC or CLANG

## Clone & Compiling

```sh
git clone --recurse-submodules https://github.com/nempk1/cchat.git
cd cchat
make
```
If you want to choose the compiler

```sh
CC=gcc make
# OR
CC=clang make
```

And pray that you will not get an error or that libconfig is not found.
if libconfig is not found try adding it to your LD_LIBRARY_PATH
and check if the includes are being located to.

## Config & Usage

1. Open the sample cchat.cfg file in the configs/ directory
1. Add the channel, oauth and oauth_nick variables that are necessary
2. if you want you can choose the channel to join in the running arguments.
1. After that just run it
```sh
./bin/cchat -i irc.chat.twitch.tv -p 6697 -c configs/cchat.cfg
# You change use the --channel argument to change the chat to join
# without need to change in the file.
./bin/cchat -i irc.chat.twitch.tv -p 6697 -c configs/cchat.cfg --channel nempk1
```
