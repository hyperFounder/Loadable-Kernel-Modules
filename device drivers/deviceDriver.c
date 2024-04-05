#include <linux/init.h>
#include <linux/module.h>
#include <linux/fs.h>
#include <linux/uaccess.h>

#define DEVICE_NAME "mychardev"  // Device name

static int major_number;  // Device major number
static int numberTimes = 0;

// Device open function
static int mychardev_open(struct inode *inode, struct file *file) {
    numberTimes += 1;
    printk(KERN_INFO "Device opened %ld times\n", n);
    return 0;
}

// Device close function
static int mychardev_release(struct inode *inode, struct file *file) {
    printk(KERN_INFO "Device closed\n");
    return 0;
}

// Device read function
static ssize_t mychardev_read(struct file *file, char __user *user_buffer, size_t count, loff_t *offset) {
    char message[] = "Hello from the kernel!\n";
    size_t message_len = strlen(message);

    if (copy_to_user(user_buffer, message, message_len) != 0) {
        return -EFAULT;
    }

    return message_len;
}

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


// File operations structure
static struct file_operations fops = {
    .open = mychardev_open,
    .release = mychardev_release,
    .read = mychardev_read,
    .write = mychardev_write,
};

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

// Module cleanup function
static void __exit mychardev_exit(void) {
    // Unregister device
    unregister_chrdev(major_number, DEVICE_NAME);
    printk(KERN_INFO "Unregistered device\n");
}

module_init(mychardev_init);
module_exit(mychardev_exit);

MODULE_LICENSE("GPL");
MODULE_DESCRIPTION("Simple character device driver");
