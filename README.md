# Loadable-Kernel-Modules
Loadable kernel modules, kernel mutexes (mutual exclusions), spinlocks, etc.

### Tutorials and documentation

- [Linux module kernel programming](https://sysprog21.github.io/lkmpg/)
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

### The proc filesystem and Kernel and User space
- The ```/proc``` is the process directory and acts as an interface between user and kernel spaces.
- - User processes can read from and write to the files in ```/proc``` directory
  - The kernel will then handle these read/writes and perform the requested actions.
---
- Let's take a look at this kernel module. These are our header files.
```c
#include <linux/module.h>
#include <linux/kernel.h>
#include <linux/proc_fs.h>
#include <linux/uaccess.h>
```
- Let's now create a ```/proc``` directory entry such that user processes can read from/write to the these files.

```c
static struct proc_dir_entry* proc_entry;

static ssize_t custom_read(struct file* file, char __user* user_buffer, size_t count, loff_t* offset)
{
    printk(KERN_INFO "calling our very own custom read method.");
    char greeting[] = "Hello world!\n";h
    int greeting_length = strlen(greeting);
    if (*offset > 0)
        return 0;
    if (copy_to_user(user_buffer, greeting, greeting_length))
        return -EFAULT;
    *offset = greeting_length;
    return greeting_length;
}

static const struct proc_ops proc_fops = {
    .proc_read = custom_read // reading from a /proc entry
};
```
- Let's now initialise our kernel module.
```c
static int __init custom_init(void) {
    proc_entry = proc_create("helloworlddriver", 0666, NULL, &proc_fops);
    if (!proc_entry) {
        printk(KERN_ERR "Failed to create proc entry\n");
        return -ENOMEM;
    }
    printk(KERN_INFO "Hello world driver loaded.");
    return 0;
}

static void __exit custom_exit(void) {
    remove_proc_entry("helloworlddriver", NULL);
    printk(KERN_INFO "Goodbye my friend, I shall miss you dearly...");
}

module_init(custom_init);
module_exit(custom_exit);
```
- Now, load our kernel module:
```c
sudo insmod shared.ko
```
- Run ```cat /proc/helloworlddriver```, and you should see our driver returning the ```Hello World``` greeting to the terminal.
---
- Let’s write our own user space application to interact with this driver.
```python
kernel_module = open('/proc/helloworlddriver')
greeting = kernel_module.readline();
print(greeting)
kernel_module.close()
```
-  Now, if you run ```python3 hello.py```, you should see the ```Hello World``` greeting to the terminal.
-  In our code, we made a ```custom read``` function. As you might guess, you can override the write function as well, if your module requires some userspace input. For instance, if you had a driver that controls the speed of the fans in your PC, you could give it a write function where you write a percentage number between 0 and 100 to the file, and your driver manually adjusts the fan speed accordingly.
