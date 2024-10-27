//module will load successfully, and the init msg will be printed.
//unloading this module with rmmod will fail,becoz there's no cleanup function to remove it cleanly.

#include <linux/module.h>
#include <linux/kernel.h>

int init__module(void)
{
        printk("Module with only inti function loaded\n");
        return 0;
}

module_init(init__module);

MODULE_LICENSE("GPL");
MODULE_AUTHOR("sonia");
MODULE_DESCRIPTION("A module with only init function");
