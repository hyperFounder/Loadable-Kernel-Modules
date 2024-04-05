#include <linux/init.h>
#include <linux/module.h>
#include <linux/kernel.h>
#include <linux/proc_fs.h>
#include <linux/uaccess.h>
#include <linux/mutex.h>

static DEFINE_MUTEX(counter_mutex); // Mutex for counter access
static DEFINE_SPINLOCK(counter_lock); // Alternative to Mutexes: Spinlock for counter access

static long counter_value = 0;

// Increment the counter
static ssize_t counter_increment(void) {
  mutex_lock(&counter_mutex);
  counter_value++;
  printk(KERN_INFO "Counter value: %ld", counter_value);
  mutex_unlock(&counter_mutex);
  return 0;
}

// Decrement the counter
static ssize_t counter_decrement(void) {
  mutex_lock(&counter_mutex);
  counter_value--;
  printk(KERN_INFO "Counter value: %ld", counter_value);
  mutex_unlock(&counter_mutex);
  return 0;
}
// Write the counter from user memory space to the kernel memory space
static ssize_t counter_write(struct file *file, const char __user *user_buffer,
                             size_t count, loff_t *f_pos) {
  if (!access_ok(user_buffer, count)) {
    return -EFAULT;
  }

  char cmd;
  if (copy_from_user(&cmd, user_buffer, sizeof(cmd))) {
    return -EFAULT;
  }

  switch (cmd) {
    case 'I':
      counter_increment();
      break;
    case 'D':
      counter_decrement();
      break;
    default:
      return -EINVAL; // Invalid command
  }

  return count; // Return after processing the command
}

static const struct proc_ops counter_ops = {
  .proc_write = counter_write 
};

static struct proc_dir_entry *proc_entry; 

static int __init counter_init(void) {
  proc_entry = proc_create("counter", 0666, NULL, &counter_ops);
  if (!proc_entry) {
    printk(KERN_ALERT "Failed to create proc entry\n");
    return -ENOMEM;
  }
  printk(KERN_INFO "Counter module loaded\n");
  return 0;
}

static void __exit counter_exit(void) {
  proc_remove(proc_entry);
  printk(KERN_INFO "Counter module unloaded\n");
}

module_init(counter_init);
module_exit(counter_exit);

MODULE_LICENSE("GPL");
MODULE_DESCRIPTION("Counter driver with user-kernel communication");
