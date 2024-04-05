## Device Drivers

A Loadable kernel module is called a driver. It's kind of like an API that allows the communication between user space programs and the kernel.

- Device drivers are kernel modules (i.e: software code) that act as an interface between hardware devices and the OS of the operating system, enabling communication between OS and hardware.
---
### The ```/dev``` directory: A directory that contains device files

- Each hardware device typically has its own device file.
- These device files allow user-space programs to interact with the hardware as if it were a file. For example, reading from the device file can trigger operations on the corresponding hardware.

### Block and Character devices

- Devices are divided into two types: character devices and block devices.
- A character device typically transfers data one char at a time without buffering (character-by-character stream).
- Block devices have a buffer for requests, so they can choose the best order in which to respond to the requests.
- - They also transfer data in fixed-size blocks (e.g., sectors).
---
### Distinguishing between character and block devices
- Let’s look at some device files. ```c``` indicates that this is a **character** device file.
- - A ```b``` would indicate a **block** device file.
```
ryan@ryan-VirtualBox:~/Desktop/shared$ ls -l /dev/zero
crw-rw-rw- 1 root root 1, 5 Apr  5 01:33 /dev/zero
ryan@ryan-VirtualBox:~/Desktop/shared$ 
```
---
#### Major vs Minor number
- Major number: used by the kernel to identify the correct device driver when the device is accessed.
- Minor number: The role of the minor number is device dependent, and is handled internally within the driver.

- Consider the following two device files:
```
$ ls -l /dev/sda /dev/sdb
brw-rw---- 1 root disk 8,  0 Jan  3 09:02 /dev/sda
brw-rw---- 1 root disk 8, 16 Jan  3 09:02 /dev/sdb
```
- You can look at these two device files and know instantly that they are block devices and are handled by same driver (block major 8).
---
### Device Drivers
- ```file_operations fops``` is commonly used in Linux kernel programming for implementing device drivers:
```c
struct file_operations fops = { 
    .read = device_read,      // This function is called whenever device is being read from user space (i.e. data is being sent from the device to the user). Hence, use copy_to_user() function.
    .write = device_write,    // This function is called whenever the device is being written to from user space (i.e. data is sent from user space to the device). Hence, use copy_from_user() function.
    .open = device_open,      // Function pointer for opening the device
    .release = device_release // Function pointer for releasing the device
};

dev_open(): Called each time the device is opened from user space.
dev_read(): Called when data is sent from the device to user space.
dev_write(): Called when data is sent from user space to the device.
dev_release(): Called when the device is closed in user space.
```
- What is ```ioctl```? It stands for "input-output" control and is used to perform device-specific input/output operations.

