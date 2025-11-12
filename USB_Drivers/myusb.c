#include <linux/kernel.h>
#include <linux/module.h>
#include <linux/init.h>

static int __init myusb_init(void)
{
    pr_info("%s: Initializing....", THIS_MODULE->name);
    return 0;
}

static void __exit myusb_exit(void)
{
    pr_info("%s: Exiting...", THIS_MODULE->name);
}

module_init(myusb_init);
module_exit(myusb_exit);

MODULE_LICENSE("GPL");
MODULE_AUTHOR("Shivu");
MODULE_DESCRIPTION("Simple USB driver...");