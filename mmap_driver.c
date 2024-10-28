// mmap and remap example
// refer -> mmap_test.c (user program) to test this module

#include <linux/module.h>
#include <linux/fs.h>
#include <linux/cdev.h>
#include <linux/io.h>
#include <linux/mm.h> // mmap & remap_pfn_range
#include <linux/slab.h> // for kmalloc and kfree

#define DEVICE_NAME "mmap_driver"
#define DEVICE_SIZE (4 * PAGE_SIZE) // 4 pages
#define PATTERN 0xAA

int major_number;
char *device_data;
struct cdev my_cdev;

static int dev_open(struct inode *inode, struct file *file)
{
        memset(device_data, PATTERN, DEVICE_SIZE);
        printk(KERN_INFO "mmap_driver: Device opened\n");
        return 0;
}

static int dev_release(struct inode *inode,struct file *file)
{
        printk(KERN_INFO "mmap_driver: Device closed\n");
        return 0;
}

static int dev_mmap(struct file *file,struct vm_area_struct *vma)
{
        unsigned long pfn;
        unsigned long offset = vma->vm_pgoff << PAGE_SHIFT;
        unsigned long size = vma->vm_end - vma->vm_start;

        if(offset >= DEVICE_SIZE)
                return -EINVAL;

        if(size > (DEVICE_SIZE - offset))
                return -EINVAL;

        pfn = virt_to_phys(device_data) >> PAGE_SHIFT;
        if(remap_pfn_range(vma,vma->vm_start, pfn, size,vma->vm_page_prot))
                return -EAGAIN;
        printk(KERN_INFO "mmap_driver: Mapped %lu bytes at offset %lu\n",size,offset);
        return 0;
}

static struct file_operations fops = {
    .open = dev_open,
    .release = dev_release,
    .mmap = dev_mmap,
};
int init__module(void)
{
        int result;
        dev_t dev;

        device_data=kmalloc(DEVICE_SIZE, GFP_KERNEL);
        if(!device_data)
        {
                printk(KERN_ERR "mmap_driver: Failed to allocate memory\n");
                return -ENOMEM;
        }

        // initialize device memory with 0xAA
        memset(device_data, PATTERN, DEVICE_SIZE);

        result = alloc_chrdev_region(&dev,0,1,DEVICE_NAME);

        if(result < 0)
        {
                kfree(device_data);
                printk(KERN_WARNING "mmap_driver:Can't get major number\n");
                return result;
        }

        major_number = MAJOR(dev);
        cdev_init(&my_cdev,&fops);
        my_cdev.owner = THIS_MODULE;

        result = cdev_add(&my_cdev,dev,1);

        if(result)
        {
                kfree(device_data);
                unregister_chrdev_region(dev,1);
                printk(KERN_NOTICE "mmap_driver:Error %d adding device\n",result);
                return result;
        }
        printk(KERN_INFO "mmap_driver:Registered with major number %d\n",major_number);
        return 0;
}

void exit__module(void)
{
        cdev_del(&my_cdev);
        unregister_chrdev_region(MKDEV(major_number,0),1);
        kfree(device_data);
        printk(KERN_INFO"mmap_driver: Unregistered\n");
}

module_init(init__module);
module_exit(exit__module);

MODULE_LICENSE("R");
MODULE_AUTHOR("S");
MODULE_DESCRIPTION("mma");

