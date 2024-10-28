#include <linux/module.h>
#include <linux/kernel.h>
#include <linux/slab.h>

#define BUFFER_SIZE 256

static char *buffer;

int init__module(void)
{
        buffer = kmalloc(BUFFER_SIZE,GFP_KERNEL);
        if(!buffer)
        {
                printk(KERN_ERR "Kmalloc: failed to allocate\n");
                return -ENOMEM;
        }

        printk(KERN_INFO "kmalloc:Allocated memory\n");
        return 0;
}

void exit__module(void)
{
        if(buffer)
        {
                kfree(buffer);
                printk(KERN_INFO "freed memory\n");
        }
}

module_init(init__module);
module_exit(exit__module);

MODULE_LICENSE("GPL");
MODULE_AUTHOR("RAJI");
MODULE_DESCRIPTION("A simple kmalloc example");
