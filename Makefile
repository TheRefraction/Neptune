OBJ=floppy
PREFIX=/opt/cross
TARGET=i686-elf
GCC_VERSION=13.2.0
GDB_VERSION=14.2
BINUTILS_VERSION=2.42

export PREFIX
export TARGET

all: $(OBJ)

bootloader:
	make -C boot

krnl:
	make -C kernel

floppy: bootloader krnl
	cat boot/bootloader.bin kernel/kernel.bin /dev/zero | dd of=disk.img bs=512 count=2880

run: floppy
	make -C bochs

clean:
	rm -f *.img
	make -C boot clean
	make -C kernel clean

cross_make_prefix:
	@if [ ! -d $(PREFIX) ]; then mkdir -p $(PREFIX); fi

cross_download_binutils: cross_make_prefix
	@cd $(PREFIX); if [ ! -d binutils-$(BINUTILS_VERSION) ]; then echo "Downloading Binutils $(BINUTILS_VERSION)"; curl https://ftp.gnu.org/gnu/binutils/binutils-$(BINUTILS_VERSION).tar.gz | tar -xzf -; fi

cross_download_gdb: cross_make_prefix
	@cd $(PREFIX); if [ ! -d gdb-$(GDB_VERSION) ]; then echo "Downloading GDB $(GDB_VERSION)"; curl https://ftp.gnu.org/gnu/gdb/gdb-$(GDB_VERSION).tar.gz | tar -xzf -; fi

cross_download_gcc: cross_make_prefix
	@cd $(PREFIX); if [ ! -d gcc-$(GCC_VERSION) ]; then echo "Downloading GCC $(GCC_VERSION)"; curl https://ftp.gnu.org/gnu/gcc/gcc-$(GCC_VERSION)/gcc-$(GCC_VERSION).tar.gz | tar -xzf -; fi

cross_download: cross_download_gdb cross_download_gdb cross_download_gcc

cross_configure_binutils: cross_download_binutils
	@echo "Configuring Binutils"
	@cd $(PREFIX); if [ -d build-binutils ]; then rm -rf build-binutils; fi; mkdir build-binutils; cd build-binutils; ../binutils-$(BINUTILS_VERSION)/configure --target=$(TARGET) --prefix="$(PREFIX)" --with-sysroot --disable-nls --disable-werror

cross_configure_gdb: cross_download_gdb
	@echo "Configuring GDB"
	@cd $(PREFIX); if [ -d build-gdb ]; then rm -rf build-gdb; fi; mkdir build-gdb; cd build-gdb; ../gdb-$(GDB_VERSION)/configure --target=$(TARGET) --prefix="$(PREFIX)" --disable-werror

cross_configure_gcc: cross_download_gcc
	@echo "Configuring GCC"
	@cd $(PREFIX); if [ -d build_gcc ]; then rm -rf build-gcc; fi; mkdir build-gcc; cd build-gcc; ../gcc-$(GCC_VERSION)/configure --target=$(TARGET) --prefix="$(PREFIX)" --disable-nls --enable-languages=c,c++ --without-headers

cross_configure: cross_configure_binutils cross_configure_gdb cross_configure_gcc

cross_build_binutils: cross_configure_binutils
	@echo "Building Binutils"
	@cd $(PREFIX)/build-binutils; make; make install

cross_build_gdb: cross_configure_gdb
	@echo "Building GDB"
	@cd $(PREFIX)/build-gdb; make all-gdb; make install-gdb

cross_build_gcc: cross_configure_gcc
	@echo "Building GCC3"
	@cd $(PREFIX)/build-gcc; make all-gcc; make all-target-libgcc; make install-gcc; make install-target-libgcc

cross_build: cross_build_binutils cross_build_gdb cross_build_gcc

cross_clean:
	rm -rf $(PREFIX)/build-*

cross_purge:
	rm -rf $(PREFIX)
