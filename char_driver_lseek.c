// character driver read, write, lseek operations
#include <linux/module.h>
#include <linux/fs.h>
#include <linux/cdev.h>
#include <linux/slab.h>
#include <linux/uaccess.h>

#define DEVICE_NAME "sample_driver"
#define BUFFER_SIZE 100

static int major_number;
static int open_count = 0;
static struct cdev my_cdev;

struct my_data {
    char buffer[BUFFER_SIZE];
    size_t size;
    loff_t pos;
};

static int dev_open(struct inode *inode, struct file *file)
{
    struct my_data *data;

    data = kmalloc(sizeof(struct my_data), GFP_KERNEL);
    if (!data)
    {
        return -ENOMEM;
    }

    memset(data->buffer, 0, BUFFER_SIZE);
    data->size = 0;
    data->pos = 0;
    file->private_data = data;

    open_count++;
    printk(KERN_INFO "Device opened %d times\n", open_count);
    printk(KERN_INFO "Major: %d, Minor: %d\n", imajor(inode), iminor(inode));
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
    size_t to_read;

    if (data->pos >= BUFFER_SIZE)
    {
        return 0; // End of buffer
    }

    to_read = min(size, (size_t)(BUFFER_SIZE - data->pos));
    if (copy_to_user(user_buffer, data->buffer + data->pos, to_read))
    {
        return -EFAULT;
    }

    data->pos += to_read;
    return to_read;
}

static ssize_t dev_write(struct file *file, const char __user *user_buffer, size_t size, loff_t *offset)
{
    struct my_data *data = file->private_data;
    size_t to_write;

    if (data->pos >= BUFFER_SIZE)
    {
        return -ENOSPC; // No space left on buffer
    }

    to_write = min(size, (size_t)(BUFFER_SIZE - data->pos));
    if (copy_from_user(data->buffer + data->pos, user_buffer, to_write))
    {
        return -EFAULT;
    }

    data->pos += to_write;
    data->size = max(data->size, data->pos);
    return to_write;
}
static loff_t dev_lseek(struct file *file, loff_t offset, int whence)
{
    struct my_data *data = file->private_data;
    loff_t new_pos;

    switch (whence)
    {
    case SEEK_SET:
        new_pos = offset;
        break;
    case SEEK_CUR:
        new_pos = data->pos + offset;
        break;
    case SEEK_END:
        new_pos = data->size + offset;
        break;
    default:
        return -EINVAL;
    }

    if (new_pos < 0 || new_pos > BUFFER_SIZE)
    {
        return -EINVAL;
    }

    printk(KERN_INFO "Seeking to position %lld\n", new_pos);

    data->pos = new_pos;
    return new_pos;
}

static struct file_operations fops = {
    .open = dev_open,
    .release = dev_release,
    .read = dev_read,
    .write = dev_write,
    .llseek = dev_lseek,
};

static int __init sample_driver_init(void)
{
    int result;
    dev_t dev;

    result = alloc_chrdev_region(&dev, 0, 1, DEVICE_NAME);
    if (result < 0)
    {
        printk(KERN_WARNING "Can't get major number\n");
        return result;
    }

    major_number = MAJOR(dev);
    cdev_init(&my_cdev, &fops);
    my_cdev.owner = THIS_MODULE;

    result = cdev_add(&my_cdev, dev, 1);
    if (result)
    {
        printk(KERN_NOTICE "Error %d adding sample_driver\n", result);
        unregister_chrdev_region(dev, 1);
        return result;
    }

    printk(KERN_INFO "Sample driver registered: %d\n", major_number);
    return 0;
}

static void __exit sample_driver_exit(void)
{
    cdev_del(&my_cdev);
    unregister_chrdev_region(MKDEV(major_number, 0), 1);
    printk(KERN_INFO "Sample driver unregistered\n");
}

module_init(sample_driver_init);
module_exit(sample_driver_exit);

MODULE_LICENSE("GPL");
MODULE_AUTHOR("RAJI");
MODULE_DESCRIPTION("A sample driver with read, write, and lseek operations");
