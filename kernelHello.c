#include <linux/module.h>  /* Needed by all modules */
#include <linux/kernel.h>  /* Needed for KERN_ALERT */

static int __init custom_init(void){
  printk(KERN_INFO "Hello, kernel world!");
  return 0; //A non 0 return means init_module failed; module can't be loaded.
}

static void __exit custom_exit(void){
  printk(KERN_INFO "Goodbye, kernel world!");
}

module_init(custom_init);
module_exit(custom_exit);

// METADATA
MODULE_LICENSE("GPL");
