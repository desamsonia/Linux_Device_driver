//Allocating the memory in kernel-space and Freeing the memory
#include <linux/module.h>
#include <linux/sched.h>
#include <linux/fs.h>
#include <linux/slab.h>
#include <linux/kernel.h>

int *ptr;
int Memory_init(void)
{
        int i;
        printk("Loading a module to the kernel\n");
        printk("Allocating the memory in kernel-space\n");

        ptr = (int*)kmalloc(100,GFP_KERNEL);
        for(i=0;i<10;i++)
        {
                *(ptr+i) = i+1;
        }

        printk("The values are\n");
        for(i=0;i<10;i++)
        {
                printk("%d\n",*(ptr+i));
        }
        return 0;
}

void Memory_exit(void)
{
        printk("Unloading a module from the kernel\n");
        printk("Freeing the memory\n");
        kfree(ptr);
}

module_init(Memory_init);
module_exit(Memory_exit);
MODULE_LICENSE("DMA");
MODULE_AUTHOR("MEMORY");
MODULE_DESCRIPTION("MEMORY ALLOCATION");
