# Loadable-Kernel-Modules
Loadable kernel modules, kernel mutexes (mutual exclusions), spinlocks, etc.

### Tutorials and documentation

- [Linux kernel programming](https://sysprog21.github.io/lkmpg/)
- [Writing a Linux Kernel Module — Part 1: Introduction](http://derekmolloy.ie/writing-a-linux-kernel-module-part-1-introduction/)
- [Writing a Linux Kernel Module — Part 2: A Character Device](http://derekmolloy.ie/writing-a-linux-kernel-module-part-2-a-character-device/)
- [The Linux Kernel documentation](https://www.kernel.org/doc/html/latest/)

## What Modules are in my Kernel?

To discover what modules are already loaded within your current kernel:
```
sudo lsmod
```
- Modules are stored within the file ```/proc/modules```, so you can also see them with
```sudo cat /proc/modules```

## A simple Kernel Module

- Setup, on Ubuntu/Debian GNU/Linux:
```
sudo apt-get install build-essential
```
- Before building anything, it is necessary to search and install the header files for the kernel. In my case, Linux kernel headers for version 6.5.0 on 64 bit x86 SMP.
```c
apt-cache search linux-headers-`uname -r`
```
- I am now installing the header files for my Linux kernel and ```kmod```, the Linux kernel module management utility.
```c
sudo apt-get install kmod linux-headers-6.5.0-26-generic
```
- Finally, run the Makefile with ```make```. If all goes smoothly you should then find that you have a compiled ```kernelHello.ko``` module.
