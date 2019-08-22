#include <linux/init.h>
#include <linux/kernel.h>
#include <linux/module.h>
#include <linux/timer.h>

char* name;
unsigned long period = 500;

MODULE_LICENSE("GPL");
MODULE_AUTHOR("Nachiket");
MODULE_DESCRIPTION("First module");

module_param(name, charp, 0644);
MODULE_PARM_DESC(name,"Enter your name");
module_param(period, ulong, 0644);
MODULE_PARM_DESC(period,"Enter period in milliseconds");

static struct timer_list period_timer;

static void print_name_timer_fired(struct timer_list *data)
{
    static uint16_t no_of_times_fired;
    no_of_times_fired ++;
    printk("Your name %s and Timer was fired %d number of times",name, no_of_times_fired);
    period_timer.expires = jiffies + msecs_to_jiffies(period);
    mod_timer(&period_timer, period_timer.expires);
}


static int hello_init(void)
{
     period_timer.expires = jiffies + msecs_to_jiffies(period);
     //period_timer.data = 0;
     //period_timer.function = print_name_timer_fired;
     printk(KERN_ALERT "Module inserted successfully\n");
     timer_setup(&period_timer,print_name_timer_fired,0);
     add_timer(&period_timer);
     return 0;
}


static void hello_exit(void)
{
     printk(KERN_ALERT "Bye I am leaving this country\n");
     del_timer(&period_timer);
}


module_init(hello_init);
module_exit(hello_exit);
