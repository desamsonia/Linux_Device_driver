// without inti_module,insmod - successfully adding file,but it won't perform any meaningful initialization

#include <linux/module.h>
#include <linux/kernel.h>

static void __exit my_module_exit(void)
{
    printk(KERN_INFO "Module with only cleanup function unloaded.\n");
}

module_exit(my_module_exit);

MODULE_LICENSE("GPL");
MODULE_AUTHOR("Your Name");
MODULE_DESCRIPTION("A module with only cleanup function");
