// simple interrupt_driver

#include <linux/module.h>
#include <linux/kernel.h>
#include <linux/interrupt.h>
#include <linux/timer.h>

#define IRQ_NUM 3
#define DEVICE_NAME "interrupt_driver"

static struct timer_list my_timer;
int irq_counter = 0;

static irqreturn_t my_interrupt_handler(int irq,void *dev_id)
{
        irq_counter++;
        printk(KERN_INFO "interrrupt_driver: Interrupt occurred, count = %d\n", irq_counter);
        mod_timer(&my_timer, jiffies + msecs_to_jiffies(1000));
        return IRQ_HANDLED;
}

static void my_timer_callback(struct timer_list *t)
{
        my_interrupt_handler(IRQ_NUM, NULL);
        //raise_irq(IRQ_NUM);
}

static int __init interrupt_driver_init(void)
{
        int r;
        r=request_irq(IRQ_NUM, my_interrupt_handler,IRQF_SHARED, DEVICE_NAME,(void *)(my_interrupt_handler));
        if(r)
        {
                printk(KERN_ERR "interrupt_driver: cannot reuest IRQ %d\n",IRQ_NUM);
                return r;
        }

        timer_setup(&my_timer, my_timer_callback,0);
        mod_timer(&my_timer,jiffies + msecs_to_jiffies(1000));
        printk(KERN_INFO "interrupt_driver: Module loaded,IRQ %d requested\n",IRQ_NUM);
        return 0;
}

static void __exit interrupt_driver_exit(void)
{
        free_irq(IRQ_NUM, (void *)(my_interrupt_handler));
        del_timer(&my_timer);
        printk(KERN_INFO "module unloaded, IRQ %d freed\n",IRQ_NUM);
}

module_init(interrupt_driver_init);
module_exit(interrupt_driver_exit);
MODULE_LICENSE("GPL");
MODULE_AUTHOR("INT");
MODULE_DESCRIPTION("A simple interrupt handling example driver");

