# Loadable-Kernel-Modules
Loadable kernel modules, kernel mutexes (mutual exclusions), spinlocks, etc.

### Tutorials and documentation

- [Linux module kernel programming](https://sysprog21.github.io/lkmpg/)
- [Writing a Linux Kernel Module — Part 1: Introduction](http://derekmolloy.ie/writing-a-linux-kernel-module-part-1-introduction/)
- [Writing a Linux Kernel Module — Part 2: A Character Device](http://derekmolloy.ie/writing-a-linux-kernel-module-part-2-a-character-device/)
- [The Linux Kernel documentation](https://www.kernel.org/doc/html/latest/)
- [How to write your first Linux Kernel Module](https://medium.com/dvt-engineering/how-to-write-your-first-linux-kernel-module-cf284408beeb)

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
- Let's now create a ```/proc``` directory entry such that user processes can read from to the these files.

```c
static struct proc_dir_entry* proc_entry;


static ssize_t custom_read(struct file* file, char __user* user_buffer, size_t count, loff_t* offset)
{
    printk(KERN_INFO "calling our very own custom read method.");
    char buffy[] = "Hello world!\n";h // Assuming 'buffy' is a kernel space buffer containing data.
    int n = strlen(buffy); // Buffer length
    if (*offset > 0)
        return 0;
    // copy_to_user function is a function to copy data from kernel memory (buffy) to user space (user_buffer)
    if (copy_to_user(user_buffer, buffy, n))
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
---
## A mutex counter application: counter.c

- We shall implement a kernel module that maintains a shared counter varible that can be either incremented or decremented.
- To increment a counter varible:
```c
echo I > /proc/counter
```
- To decrement the counter variable:
```c
echo D > /proc/counter
```
-   This requires creating a proc file named ```/proc/counter``` to interact with the counter maintained by the kernel module.
-   Given the fact that we have not implemented a ```.proc_write```, we cannot **read** from the kernel.
- Why mutexes? If we're using threads to modify the shared variable ```counter```, then the ```mutex locks``` will serialise access of the threads to update the shared variable.
- The ```copy_from_user()``` function copies the data from the user space into kernel memory.

---
### Setup Instructions 

- Run the Makefile
```c
make
```
- Create and insert the module in the ```/proc``` directory
```c
sudo insmod counter.ko
```
- Visualising counter output: ```dmesg``` is used to print or view the messages from the kernel's ring buffer.
```c
sudo dmesg
```
