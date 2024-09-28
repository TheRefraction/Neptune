#!/bin/bash
echo "Setting environment variable for current shell session"
export PREFIX="$HOME/src/opt/cross"
export TARGET=i686-elf
export PATH="$PREFIX/bin:$PATH"
export GCC_VERSION=13.2.0
export GDB_VERSION=14.2
export BINUTILS_VERSION=2.42
