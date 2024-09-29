# Neptune

Neptune (_also known as N/OS_) is a small operating system made in Assembly and C. 

## Building N/OS

There are multiple steps in order to build N/OS.
1. [Dependencies](#dependencies)
2. [Bootloader](#bootloader)
3. [Kernel](#kernel)
4. [Boot Media Generation](#media-gen)

---

### Dependencies <a name="dependencies"></a>

Download and install the following dependencies if you don't have them already.

- A 32-bit GCC Cross Compiler (_i686_elf_) 
	- build-essential
	- bison
	- flex
	- libgmp3-dev
	- libmpc-dev
	- libmpfr-dev
	- texinfo
	- libisl-dev
- [Netwide Assembler (nasm)](https://www.nasm.us)
- [Git](https://git-scm.com/downloads)

NOTE: The GCC Cross Compiler can be built using the bash script (*Unix only*) ```./build_cross.sh```. Recommended versions are:
- GCC 13.2.0
- GDB 14.2
- Binutils 2.42
If you decide to compile your cross compiler by yourself, change the ```$PREFIX``` to your chosen directory.

---

### Bootloader <a name="bootloader"></a>

TODO

---

### Kernel <a name="kernel"></a>

TODO

---

### Boot Media Generation <a name="media-gen"></a>

TODO

---

## Running N/OS

TODO

---

## Directory Explanation

	- bochs: Bochs configuration files
	- boot: Bootloader source code
	- doc: Documentation
	- kernel: Kernel source code

---
