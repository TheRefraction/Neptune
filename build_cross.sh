#!/bin/bash
echo "Building GCC Cross-compiler"
read -p "Press [ENTER] to start..."
source ./set_var.sh
export OLD=$PWD

if [ ! -d "$PREFIX" ]; then
	mkdir $PREFIX
fi

cd $PREFIX
if [ ! -d "binutils-$BINUTILS_VERSION" ]; then
	echo "Downloading Binutils"
	curl https://ftp.gnu.org/gnu/binutils/binutils-$BINUTILS_VERSION.tar.gz | tar -xzf -
fi

echo "Preparing Binutils"
if [ ! -d "build-binutils" ]; then
	mkdir build-binutils
else
	rm -rf build-binutils
fi
cd build-binutils
../binutils-$BINUTILS_VERSION/configure --target=$TARGET --prefix="$PREFIX" --with-sysroot --disable-nls --disable-werror

echo "Building & Installing Binutils"
make
make install

read -p "Press [ENTER] to continue..."
cd $PREFIX
if [ ! -d "gdb-$GDB_VERSION" ]; then
	echo "Downloading GDB"
	curl https://ftp.gnu.org/gnu/gdb/gdb-$GDB_VERSION.tar.gz | tar -xzf - 
fi

echo "Preparing GDB"
if [ ! -d "build-gdb" ]; then
	mkdir build-gdb
else
	rm -rf build-gdb
fi
cd build-gdb
../gdb-$GDB_VERSION/configure --target=$TARGET --prefix="$PREFIX" --disable-werror

echo "Building & Installing GDB"
make all-gdb
make install-gdb

read -p "Press [ENTER] to continue..."
cd $PREFIX
if [ ! -d "gcc-$GCC_VERSION" ]; then
	echo "Downloading GCC"
	curl https://ftp.gnu.org/gnu/gcc/gcc-$GCC_VERSION/gcc-$GCC_VERSION.tar.gz | tar -xzf - 
fi

echo "Preparing GCC"

which -- $TARGET-as || echo $TARGET-as is not in the PATH

if [ ! -d "build-gcc" ]; then
	mkdir build-gcc
else
	rm -rf build-gcc
fi
cd build-gcc
../gcc-$GCC_VERSION/configure --target=$TARGET --prefix="$PREFIX" --disable-nls --enable-languages=c,c++ --without-headers

echo "Building & Installing GCC"
make all-gcc
make all-target-libgcc
make install-gcc
make install-target-libgcc

cd $OLD
echo "Done!"
