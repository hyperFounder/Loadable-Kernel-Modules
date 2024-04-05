#include <linux/init.h>
#include <linux/module.h>
#include <linux/proc_fs.h>
#include <linux/seq_file.h>
#include <linux/slab.h>
#include <linux/spinlock.h>

struct list_node {
    int data;
    struct list_node *next;
};

static struct list_node *head = NULL;
static DEFINE_SPINLOCK(list_lock);

// Function to add data to the linked list
static void add_data(int data) {
    struct list_node *new_node = kmalloc(sizeof(struct list_node), GFP_KERNEL);
    if (!new_node) {
        printk(KERN_ERR "Failed to allocate memory for new node\n");
        return;
    }
    spin_lock(&list_lock);
    new_node->data = data;
    new_node->next = head;
    head = new_node;
    spin_unlock(&list_lock);
}


// This is a function that allows us to open the proc file
static int linkedlist_proc_open(struct inode *inode, struct file *file) {
    // Each file on a filesystem is represented by an inode. Contains file permissions, file sizes, etc.
    return single_open(file, linkedlist_proc_show, NULL);
}

// Function to display the linked list contents in the proc file
static int linkedlist_proc_show(struct seq_file *m, void *v) {
    struct list_node *current_node;
    spin_lock(&list_lock);
    for (current_node = head; current_node != NULL; current_node = current_node->next) {
        seq_printf(m, "%d\n", current_node->data);
    }
    spin_unlock(&list_lock);
    return 0;
}

//  A structure which defines what can be performed on the proc file.
static const struct proc_ops linkedlist_proc_fops = {
    .proc_open = linkedlist_proc_open,
    .proc_read = seq_read,
    .proc_lseek = seq_lseek,
    .proc_release = single_release,
};

// Module initialization function
static int __init linkedlist_init(void) {
    // Populate the linked list with dummy data
    add_data(1);
    add_data(2);
    add_data(3);

    // Create proc file
    proc_create("linkedlist", 0, NULL, &linkedlist_proc_fops);

    printk(KERN_INFO "Linkedlist module loaded\n");
    return 0;
}

// Module exit function
static void __exit linkedlist_exit(void) {
    // Remove proc file
    remove_proc_entry("linkedlist", NULL);
    printk(KERN_INFO "Linkedlist module unloaded\n");
}

module_init(linkedlist_init);
module_exit(linkedlist_exit);

MODULE_LICENSE("GPL");
MODULE_DESCRIPTION("Kernel module with simple linked list");
