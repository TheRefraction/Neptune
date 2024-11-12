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

- A 32-bit GCC Cross Compiler (_i686-elf_) 
	- Compiler
    - Make
	- [Bison](https://www.gnu.org/software/bison)
	- [Flex](https://github.com/westes/flex)
	- [GMP](https://gmplib.org)
	- MPC
	- [MPFR](https://www.mpfr.org)
	- [Texinfo](https://www.gnu.org/software/texinfo)
	- ISL
- [Netwide Assembler (nasm)](https://www.nasm.us)
- [Git](https://git-scm.com/downloads)

NOTE: All required dependencies can be installed by running the following shell command:
- On Debian distributions ```sudo apt install build-essential bison flex libgmp3-dev libmpc-dev libmpfr-dev texinfo libisl-dev nasm git```
- On Arch ```pacman -Syu base-devel gmp libmpc mpfr nasm git```

NOTE: The GCC Cross Compiler can be built using the main Makefile (*Unix only*) ```make build_cross```. 
<br>
You may choose where the cross compiler resides by changing the ```PREFIX``` variable. Please keep in mind that depending on what you set ```PREFIX``` to, you may need to run the Makefile with root privileges. Moreover building it usually takes **around 40 minutes**.
<br>
Default ```TARGET```is set to ```i686-elf```. 
<br>
Versions can be changed through the Makefile. Recommended versions are:
- GCC 13.2.0
- GDB 14.2
- Binutils 2.42

---

### Bootloader <a name="bootloader"></a>

TODO

---

### Kernel <a name="kernel"></a>

TODO

---

### Boot Media Generation <a name="media-gen"></a>

As of now it is possible to generate a floppy disk image (.img) using the following command ```make floppy```. 
<br>
By default the ```make``` command will compile and generate a floppy disk image. 
This can be changed by setting the ```OBJ``` variable to the chosen medium if it exists.

---

## Running N/OS

The generated disk image can be opened with either Bochs or QEMU. For debugging purposes, Bochs is recommended. You may have to install ```bochs-x``` in order to display the window and debugging GUI. Configuration files and BIOS images are saved onto the bochs folder.
<br>
Run the following command ```make run```.

---

## Directory Explanation

	- bochs: Bochs configuration files
	- boot: Bootloader source code
	- doc: Documentation
	- kernel: Kernel source code

---
