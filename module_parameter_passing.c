//write a module that can take an integer parameter when it is loaded with insmod . it should have default value when none is specified. load it and unload it. while the module is loaded look at its directory in /sys/module/ and see if you can change the value of parameter you established. use c code

#include <linux/module.h>
#include <linux/kernel.h>
#include <linux/init.h>
#include <linux/moduleparam.h>

static int my_param = 42;  // Default value
module_param(my_param, int, S_IRUGO | S_IWUSR);
MODULE_PARM_DESC(my_param, "An integer parameter");

static int __init my_module_init(void)
{
    printk(KERN_INFO "Loading module with parameter: %d\n", my_param);
    return 0;
}

static void __exit my_module_exit(void)
{
    printk(KERN_INFO "Unloading module\n");
}

module_init(my_module_init);
module_exit(my_module_exit);

MODULE_LICENSE("GPL");
MODULE_AUTHOR("Your Name");
MODULE_DESCRIPTION("A simple module with a parameter");
