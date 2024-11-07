/*write a module that shares its IRQ with your network card and check proc/interrupts 
while it is loaded and have the module keep track of the no of times the interrupt handler gets called */

#include <linux/module.h>
#include <linux/kernel.h>
#include <linux/interrupt.h>
#include <linux/netdevice.h>
#include <linux/proc_fs.h>

#define DEVICE_NAME "shared_irq"

int irq_counter = 0;
int irq_number = 19; // in my system IRQ -> enp0s3

static irqreturn_t irq_handler(int irq, void *dev_id)
{
        irq_counter++;
        printk(KERN_INFO "shared_irq_module: Interrupt occurred, count = %d\n",irq_counter);
        return IRQ_NONE;
}

static int __init shared_irq_module_init(void)
{
        int r;
        r=request_irq(irq_number,irq_handler,IRQF_SHARED,DEVICE_NAME, (void *)(irq_handler));
        if(r)
        {
                printk(KERN_ERR "shared_irq: cannot request IRQ %d\n", irq_number);
                return r;
        }

        printk(KERN_INFO "shared_irq: module loaded, IRQ %d requested\n",irq_number);
        return 0;
}

static void __exit shared_irq_module_exit(void)
{
        free_irq(irq_number,(void *)(irq_handler));
        printk(KERN_INFO "shared_irq: module unloaded, IRQ %d freed\n",irq_number);
}

module_init(shared_irq_module_init);
module_exit(shared_irq_module_exit);

MODULE_LICENSE("s");
MODULE_AUTHOR("RP");
MODULE_DESCRIPTION("A kernel module that shared an IRQ with a network card and tracks interrupt count");
