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
### A Linux kernel device driver
- ```file_operations fops``` is commonly used in Linux kernel programming for implementing device drivers:
```c
struct file_operations fops = { 
    .read = device_read,      // Read from the device (so data is being sent from the device to the user). Hence, use copy_to_user().
    .write = device_write,    // This function is called whenever the device is being written to from user space (i.e. data is sent from user space to the device). Hence, use copy_from_user() function.
    .open = device_open,      //  This is called each time the device is opened from user space.
    .release = device_release //  This is called when the device is closed in user space.
};

```
- What is ```ioctl```? It stands for "input-output" control and is used to perform device-specific input/output operations.

- The device open function is called each time the device is opened.
- - A counter is incremented.
```c
// Device open function
static int mychardev_open(struct inode *inode, struct file *file) {
    numberTimes += 1;
    printk(KERN_INFO "Device opened %ld times\n", n);
    return 0;
}
```
- This function is called when we read from the device.
- - i.e: data is being sent from the device to the user. Hence, use ```copy_to_user()```.
```c
/ Device read function
static ssize_t mychardev_read(struct file *file, char __user *user_buffer, size_t count, loff_t *offset) {
    char message[] = "Hello from the kernel!\n";
    size_t message_len = strlen(message);
    if (copy_to_user(user_buffer, message, message_len) != 0) {
        return -EFAULT;
    }
    return message_len;
}
```
- This function is called whenever the device is being written to from user space.
- - i.e. data is sent from user space to the device. Hence, use ```copy_from_user()``` function.
```c
// Device write function
static ssize_t mychardev_write(struct file *file, const char __user *user_buffer, size_t count, loff_t *offset) {
    char kernel_buffer[256];  // Buffer to hold the data from userspace
    // Check if the provided buffer address is valid
    if (!user_buffer) {
        return -EINVAL;
    }
    // Copy data from userspace to kernel buffer
    if (copy_from_user(kernel_buffer, user_buffer, count) != 0) {
        return -EFAULT;
    }
    // Print the data received from userspace
    printk(KERN_INFO "Received data from userspace: %s\n", kernel_buffer);
    // Return the number of bytes written
    return count;
}
```
- This is our initialisation function. We need to registed the device and assign a ```major number```.
```c
// Module initialization function
static int __init mychardev_init(void) {
    // Register device
    major_number = register_chrdev(0, DEVICE_NAME, &fops);
    if (major_number < 0) {
        printk(KERN_ALERT "Failed to register a major number\n");
        return major_number;
    }
    printk(KERN_INFO "Registered device with major number %d\n", major_number);
    return 0;
}
```
- Then, our cleanup function. We need to unregister the device.
```c
// Module cleanup function
static void __exit mychardev_exit(void) {
    // Unregister device
    unregister_chrdev(major_number, DEVICE_NAME);
    printk(KERN_INFO "Unregistered device\n");
}
```
---
### Setup instructions

- Create the device file using ```mknod```.
```c
sudo mknod /dev/mychardev c MAJOR_NUMBER 0
```
- To send a message from user space to the kernel:
- - Use ```sudo dmesg``` to view the corresponding output
```c
echo "Hello from user space" | sudo tee /dev/mychardev
```
- To access (read from) the device file created ```/dev/mychardev``` and interact with the driver:
```c
sudo cat /dev/mychardev
```
### Automatic recognition of hardware devices

- The operating system's kernel scans the hardware buses (such as USB, PCI, etc.) periodically or in response to specific events to detect newly connected devices.
- It then retrieves the identification information from each device (i.e: Vendor ID, Product ID, device type, etc)) and matches it against a database of known device drivers.
- **Device Driver Binding:** If a matching device driver is found for a newly detected device, the kernel binds the driver to the device.
- - This involves initializing the device driver and creating a device file in the ```/dev``` directory to represent the device to user space.
---

### GPUs and Device drivers

- GPUs (Graphics Processing Units) are specialized hardware components designed to accelerate graphics rendering and processing tasks.
- Device drivers for GPUs provide an interface for the operating system to access and control the GPU (set CPU temperature, CPU scheduling, memory management, etc).
- **Security implications**: GPU drivers can contain bugs and vulnerabilities that may be exploited by attackers to gain unauthorized access to system resources, execute arbitrary code, escalate privileges, etc.
