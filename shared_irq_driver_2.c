/*extend the previous solution (shared_irq_driver_1.c) to construct a character driver that shared every possible interrupt with already installed handlers. 
the highest interrupt number you have to consider will depend on your kernel and platform: look at /proc/interrupts to ascertain what is necessay. 
Take particular care when you call free_irq() as it is very easy to freeze your system if you are not careful. the character driver can be very simple; 
for instance if no open() and release() methods are specifies, success is default. 
a read() on the device should return a brief report on the total no of interrupts handled for each IRQ.
to do this you'll also hae to write short application to retrieve and print out the data. 
(don't forget to create the device node before you run the application.) */ 

#include <linux/module.h>
#include <linux/kernel.h>
#include <linux/interrupt.h>
#include <linux/fs.h>
#include <linux/uaccess.h>
#include <linux/slab.h>
#include <linux/device.h>

#define DEVICE_NAME "shared_irq_driver"
#define MAX_IRQ 256

int irq_counter[MAX_IRQ] = {0};
int major_number;
struct class* irq_class = NULL;
struct device* irq_device = NULL;

static irqreturn_t irq_handler(int irq,void *dev_id)
{
        irq_counter[irq]++;
        return IRQ_NONE;
}

static ssize_t irq_read(struct file *file, char __user *buf, size_t len,loff_t *offset)
{
        char *output;
        int output_len = 0,i;
        ssize_t ret;

        output = kzalloc(4096,GFP_KERNEL);
        if(!output)
                return -ENOMEM;

        for(i=0;i<MAX_IRQ;i++)
        {
                if(irq_counter[i] > 0)
                {
                        output_len += sprintf(output+output_len,"IRQ %d: %d\n",i,irq_counter[i]);
                }
        }

        if(*offset >= output_len)
        {
                kfree(output);
                return 0;
        }

        ret = copy_to_user(buf, output + *offset, len) ? -EFAULT : min(len, (size_t)(output_len - *offset));

        if(ret >= 0)
        {
                *offset += ret;
        }
        kfree(output);
        return ret;
}

static struct file_operations fops = {
        .owner = THIS_MODULE,
        .read = irq_read,
};

static int __init shared_irq_driver_init(void)
{
        int result,irq;
        major_number = register_chrdev(0,DEVICE_NAME, &fops);
        if(major_number < 0)
        {
                printk(KERN_ERR "shared_irq_driver: failed to register a major number\n");
                return major_number;
        }
        irq_class = class_create(DEVICE_NAME); //DEVICE_NAME);
        if(IS_ERR(irq_class))
        {
                unregister_chrdev(major_number, DEVICE_NAME);
                printk(KERN_ERR"shared irq driver: failed to register device class\n");
                return PTR_ERR(irq_device);
        }
        for(irq=0;irq<MAX_IRQ;irq++)
        {
                result = request_irq(irq,irq_handler,IRQF_SHARED, DEVICE_NAME,(void *)(irq_handler));
                if(result)
                {
                        printk(KERN_WARNING "shared_irq_driver: cannot request IRQ %d\n",irq);
                }
        }
        printk(KERN_INFO"shared_irq_driver: Module loaded\n");
        return 0;
}
static void __exit shared_irq_driver_exit(void)
{
        int irq;
        for(irq = 0;irq<MAX_IRQ;irq++)
        {
                free_irq(irq,(void *)(irq_handler));
        }

        device_destroy(irq_class,MKDEV(major_number,0));
        class_unregister(irq_class);
        class_destroy(irq_class);
        unregister_chrdev(major_number,DEVICE_NAME);
        printk(KERN_INFO"shared_irq_driver: Module unloaded\n");
}

module_init(shared_irq_driver_init);
module_exit(shared_irq_driver_exit);

MODULE_LICENSE("GPL");
MODULE_AUTHOR("raj");
MODULE_DESCRIPTION("A kernel module that shares every possible interrupt and tracks interrupt counts");


