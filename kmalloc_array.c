#include <linux/module.h>
#include <linux/kernel.h>
#include <linux/slab.h>

#define s 100

int *arr;

int init__module(void)
{
        int i;
        arr = kmalloc(s*sizeof(int),GFP_KERNEL);
        if(!arr)
        {
                printk(KERN_ERR "failed to allocate\n");
                return -ENOMEM;
        }

        for(i=0;i<s;i++)
        {
                arr[i] = i;
                printk(KERN_INFO "arr[%d] = %d\n",i,arr[i]);
        }

        printk(KERN_INFO "allocation success for array\n");
        return 0;
}

void exit__module(void)
{
        if(arr)
        {
                kfree(arr);
                printk(KERN_INFO "freed array memory\n");
        }
}

module_init(init__module);
module_exit(exit__module);

MODULE_LICENSE("GPL");
MODULE_AUTHOR("SONI");
MODULE_DESCRIPTION("An array allocation example using kmalloc with element printing");
