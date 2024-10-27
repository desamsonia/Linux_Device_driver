//insmod and rmmod working fine, but no meaningful initialization nd cleanup done
//the module is loaded and unloaded successfully, but it doesn't actually do anything because it lacks the necessary functions to interact with the kernel.

#include <linux/module.h>
#include <linux/kernel.h>

MODULE_LICENSE("GPL");
MODULE_AUTHOR("Your Name");
MODULE_DESCRIPTION("A module without init and cleanup functions");
