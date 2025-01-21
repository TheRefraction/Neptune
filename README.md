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
- On Arch ```sudo pacman -Syu base-devel gmp libmpc mpfr nasm git```

NOTE: The GCC Cross Compiler can be built using the main Makefile (*Unix only*) ```make cross_build```. 
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

The bootloader can be built using the ```make bootloader``` command. The Makefile outputs a binary file ```bootloader.bin``` which is then copied onto the first sectors of the medium.
<br>
N/OS bootloader is said to be a 2-stage bootloader. At that point the computer is working in *Real Mode*. The first part ```boot.bin``` weighs 512 bytes (a sector) and initializes segments to 0x07C0. Its sector is marked as bootable in the BIOS using the magic word 0x55AA (*end of sector*). It then loads the larger part of the bootloader (*stage 2, 2 sectors*) onto address 0x1000 (logical address: 10 * 0x100 (*base/selector*) + 0x0 (*offset*)) and jumps to it.
<br>
Afterwards ```loader.bin``` proceeds to reset segments, to enable the A20 Line (*allows to load bigger files in memory*), to query some information and store them at 0x7000 (*such as total memory usable*), to switch to *Protected Mode* and to load and jump onto the Kernel (*25 sectors loaded at 0x100000*).

---

### Kernel <a name="kernel"></a>

The kernel can be built using the ```make krnl``` command. The Makefile outputs a binary file ```krnl32.bin```.
<br>
N/OS kernel initializes some structures such as the GDT, the IDT and the TSS. It then initializes paging and memory management (*heaps and stacks*). Once everything is done, it loads some tasks and enable interrupts.
<br>
The kernel is assumed to take 8MiB in memory (*which is identity-mapped onto the two first pages (4MiB) of the pages directory*).

0x000000-0x0007FF : GDT
0x000800-0x000FFF : IDT
0x001000-0x001FFF : Pages directory
0x002000-0x09FFF0 : Kernel stack
0x0A0000-0x0FFFFF : Hardware (*e.g Video memory at 0xB0000*)
0x100000-0x3FFFFF : Kernel code
0x400000-0x7FFFFF : Pages tables (4MiB)
<br>

Virtual memory is organized as such:

KERNEL SPACE:
0-8 MiB: Kernel (identity-mapped)
8-16 MiB: Heap pages (limit at 0x01000000)
16-256 MiB: Free
256 MiB - 1GiB: Heap

USER SPACE (starts at 0x40000000):
???: User code
0xE0000000: User stack (limit)
0xFFFFFFFF: End of memory! (4GiB)

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
<br>
Nota Bene: By default Bochs places a breakpoint before any execution. Thus pressing the "Continue" button once allows the image to be loaded and run.

---

## Directory Explanation

	- bochs: Bochs configuration files
	- boot: Bootloader source code
	- doc: Documentation (UNUSED FOR NOW)
	- kernel: Kernel source code
        - kernel/lib : Kernel C library code (LibK)
 	- lib: Standard C library code (UNUSED FOR NOW)

---
