#include <linux/kernel.h>
#include <linux/module.h>
#include <linux/spi/spi.h>
#include <linux/init.h>
#include <linux/cdev.h>


#define DRIVER_NAME "evlsi_spi"

struct spi_device *evlsi_spi_device;
struct device  *evlsi_spi_dev;
struct cdev evlsi_cdev;
struct class *evlsi_spi_class;
dev_t dev_num;
//static uint8_t rx_buffer[1024];


int evlsi_spi_probe(struct spi_device *spi_device);
void  evlsi_spi_remove(struct spi_device *spi_device);

ssize_t evlsi_spi_read(struct file *, char __user *, size_t, loff_t *);
ssize_t evlsi_spi_write(struct file *, const char __user *, size_t, loff_t *);
int evlsi_spi_open(struct inode *, struct file *);
int evlsi_spi_release(struct inode *, struct file *);



struct spi_device_id evlsi_spi_id_table[] = {
    {"evlsi_spi", 0},
    {},
};

struct spi_driver evlsi_spi_driver = {
    .driver = {
        .name = DRIVER_NAME,
        .owner = THIS_MODULE,
    },
    .probe = evlsi_spi_probe,
    .remove = evlsi_spi_remove,
    .id_table = evlsi_spi_id_table,
};
//===========File Operations===============

struct file_operations fops = {
    .owner = THIS_MODULE,
    .read  = evlsi_spi_read,
    .write = evlsi_spi_write,
    .open = evlsi_spi_open,
    .release = evlsi_spi_release,
};

int evlsi_spi_open(struct inode *inode, struct file *filep)
{
    pr_info("%s: Calling Open...\n", THIS_MODULE->name);
    return 0;
}

int evlsi_spi_release(struct inode *inode, struct file *filep)
{
    pr_info("%s: Calling release...\n", THIS_MODULE->name);
    return 0;
}


ssize_t evlsi_spi_read(struct file *filep, char __user *touser, size_t count, loff_t *offset)
{
    pr_info("%s: Calling spi_read....\n", THIS_MODULE->name);
    return 0;
}

ssize_t evlsi_spi_write(struct file *filep, const char __user *fromuser, size_t count, loff_t *offset)
{
    pr_info("%s: Calling spi_write...", THIS_MODULE->name);
    return 0;
}

//=========================================

int evlsi_spi_probe(struct spi_device *spi_device)
{
    pr_info("%s: Probing EVLSI SPI device\n", DRIVER_NAME);
    evlsi_spi_device = spi_device;
    evlsi_spi_dev = &spi_device->dev;
    return 0;  
}

void evlsi_spi_remove(struct spi_device *spi_device)
{
    pr_info("%s: Removing EVLSI SPI device\n", DRIVER_NAME);
}

static int evlsi_spi_dev_uevent(const struct device *dev, struct kobj_uevent_env *env)
{
    add_uevent_var(env, "DEVMODE=%#o", 0666);
    return 0;
}


static int __init evlsi_spi_init(void)
{

    if(alloc_chrdev_region(&dev_num, 0, 1, DRIVER_NAME) < 0)
    {
        pr_err("%s: Failed to allocate major number\n", DRIVER_NAME);
        return -1;
    }
    cdev_init(&evlsi_cdev, &fops);
    if(cdev_add(&evlsi_cdev, dev_num, 1) < 0)
    {
        pr_err("%s: Failed to add cdev\n", DRIVER_NAME);
        unregister_chrdev_region(dev_num, 1);
        return -1;
    }
    if(IS_ERR(evlsi_spi_class = class_create("evlsi_spi_class")))
    {
        pr_err("%s: Failed to create class\n", DRIVER_NAME);
        cdev_del(&evlsi_cdev);
        unregister_chrdev_region(dev_num, 1);
        return -1;
    }
    evlsi_spi_class->dev_uevent = evlsi_spi_dev_uevent;

    if(IS_ERR(device_create(evlsi_spi_class, NULL, dev_num, NULL,DRIVER_NAME"_device")))
    {
        pr_err("%s: Failed to create device\n", DRIVER_NAME);
        class_destroy(evlsi_spi_class);
        cdev_del(&evlsi_cdev);
        unregister_chrdev_region(dev_num, 1);
        return -1;
    }

    pr_info("%s: SPI driver Initialized\n", DRIVER_NAME);

    return 0;
}

static void __exit evlsi_spi_exit(void)
{
    device_destroy(evlsi_spi_class, dev_num);
    class_destroy(evlsi_spi_class);
    cdev_del(&evlsi_cdev);
    unregister_chrdev_region(dev_num, 1);
    pr_info("%s: SPI driver exited\n", DRIVER_NAME);
}

module_init(evlsi_spi_init);
module_exit(evlsi_spi_exit);


MODULE_LICENSE("GPL");
MODULE_AUTHOR("Shivu");
MODULE_DESCRIPTION("EVLSI SPI Driver");