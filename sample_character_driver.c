//starting from sample_driver.c, extend it for keep track of number of times it has been opened since loading,and print out the counter every time the device opened and print the major and minor number when the device is opened. To exercise your driver, write a program to reda/write from the node,using standard unix i/o functions(open, close,read,write). after loading the module with insmod use this program to access the node, track usage of module by using lsmod. implment using c code
// please refer " usr_prg.c " for user program
#include <linux/module.h>
#include <linux/fs.h>
#include <linux/cdev.h>
#include <linux/uaccess.h>

#define DEVICE_NAME "sample_driver"
#define BUFFER_SIZE 100

static int major_number;
static int open_count = 0;
static struct cdev my_cdev;

struct my_data{
        char buffer[BUFFER_SIZE];
};

static int dev_open(struct inode *inode, struct file *file)
{
        struct my_data *data;
        data = kmalloc(sizeof(struct my_data), GFP_KERNEL);
        if(!data)
                return -ENOMEM;

        file->private_data = data;
        open_count++;
        printk(KERN_INFO "Device opened %d times\n",open_count);
        printk(KERN_INFO "Major: %d, Minor: %d\n",imajor(inode), iminor(inode));
        return 0;
}

static int dev_release(struct inode *inode, struct file *file)
{
        struct my_data *data = file->private_data;
        kfree(data);
        printk(KERN_INFO "Device closed\n");
        return 0;
}

static ssize_t dev_read(struct file *file, char __user *user_buffer, size_t size, loff_t *offset)
{
        struct my_data *data = file->private_data;
    size_t len = min(size, (size_t)BUFFER_SIZE);
    int ret = copy_to_user(user_buffer, data->buffer, len);
    return ret ? -EFAULT : len;
}

static ssize_t dev_write(struct file *file, const char __user *user_buffer, size_t size, loff_t *offset)
{
    struct my_data *data = file->private_data;
    size_t len = min(size, (size_t)BUFFER_SIZE);
    int ret = copy_from_user(data->buffer, user_buffer, len);
    return ret ? -EFAULT : len;
}

static struct file_operations fops = {
        .open = dev_open,
        .release = dev_release,
        .read = dev_read,
        .write = dev_write,
};

static int __init sample_driver_init(void)
{
        int result;
        dev_t dev;
        result = alloc_chrdev_region(&dev,0,1,DEVICE_NAME);
        if (result < 0)
        {
                printk(KERN_WARNING "can't get major number\n");
                return result;
        }

        major_number = MAJOR(dev);
        cdev_init(&my_cdev,&fops);
        my_cdev.owner = THIS_MODULE;

        result = cdev_add(&my_cdev,dev,1);

        if(result)
        {
                printk(KERN_NOTICE "Error %d adding sample_driver\n",result);
                unregister_chrdev_region(dev, 1);
                return result;
        }

        printk(KERN_INFO "sample driver registered: %d\n",major_number);
        return 0;
}
static void __exit sample_driver_exit(void)
{
        cdev_del(&my_cdev);
        unregister_chrdev_region(MKDEV(major_number,0),1);
        printk(KERN_INFO "sample driver unregistered\n");
}

module_init(sample_driver_init);
module_exit(sample_driver_exit);

MODULE_LICENSE("GPL");
MODULE_AUTHOR("SONIA");
MODULE_DESCRIPTION("A sample driver with open count tracking");

                                             
