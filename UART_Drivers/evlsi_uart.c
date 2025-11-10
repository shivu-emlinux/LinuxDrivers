#include <linux/module.h>
#include <linux/kernel.h>
#include <linux/init.h>
#include <linux/serial_core.h>
#include <linux/device.h>
#include <linux/kdev_t.h>

#include "uart_ioctl.h"

#define BUFFER_SIZE 1024

static dev_t dev;
static struct cdev evlsi_cdev;

static struct class *evlsi_device_class;
static uint8_t *rx_buffer;

int value = 10;


static int evlsi_uart_open(struct inode *inode, struct file *filep)
{
    pr_info("EVLSI UART Open Function Called\n");
    return 0;

}

static int evlsi_uart_release(struct inode *inode, struct file * filep)
{
    pr_info("EVLSI UART Release Function Called\n");
    return 0;
}

static long evlsi_uart_ioctl(struct file *file, unsigned int cmd, unsigned long arg)
{
    pr_info("EVLSI UART IOCTL Function Called\n");
    
    if(_IOC_TYPE(cmd) != EVLSI_UART_IOC_MAGIC)
    {
        pr_info("Invalid IOCTL Magic Number\n");
        return -EINVAL;
    }
    switch(cmd)
    {
        case READ_REGISTERS:
            pr_info("Reading Values from IOCTL\n");
            if(copy_to_user((int __user *)arg, &value, sizeof(value)) != 0)
            {
                pr_info("Failed to read registers to user space\n");
                return -EFAULT;
            }else{
                pr_info("READ_REGISTERS executed successfully\n");
            }
            break;
        case WRITE_REGISTERS:
            pr_info("Writing Values from IOCTL\n");
            if(copy_from_user(&value, (int __user *)arg, sizeof(value)) != 0)
            {
                pr_info("Failed to write registers from user space\n");
                return -EFAULT;
            }else{
                pr_info("WRITE_REGISTERS executed successfully, value=%d\n", value);
            }
            break;
        default:
            pr_info("Invalid IOCTL Command\n");
            return -EINVAL;
    }
    return 0;

}

static  ssize_t evlsi_uart_read(struct file *file, char __user *buf, size_t count, loff_t *offset)
{
    pr_info("EVLSI UART Reading\n");
    pr_info("Offset: %lld, Count:%zu, buf_address: %p, rx_buffer= %s\n", *offset, count, buf, rx_buffer);
    if(*offset >= BUFFER_SIZE){
         return 0;
    }

    if(count > BUFFER_SIZE - *offset){
        count = BUFFER_SIZE - *offset;
    }

    if(copy_to_user(buf, rx_buffer + *offset, count) != 0 ){
        pr_info("Failed to copy data to user space\n");
        return -EFAULT;
    }
    pr_info("EVLSI UART READ: %s\n", buf);
    return count;
}

static ssize_t evlsi_uart_write(struct file *file, const char __user *buf, size_t count, loff_t *offset)
{
    pr_info("EVLSI UART Write Function Called\n");
    if(*offset >= BUFFER_SIZE){
         return 0;
    }

    if(count > BUFFER_SIZE - *offset){
        count = BUFFER_SIZE - *offset;
    }

    if(copy_from_user(rx_buffer + *offset, buf, count) != 0 ){
        pr_info("Failed to write from user space to kernel space\n");
        return -EFAULT;
    }
 
    rx_buffer[count] = '\0'; 
    pr_info("EVLSI UART write : %s \n", rx_buffer);

    return count;
}

static struct file_operations fops = {
    .owner = THIS_MODULE,
    .open = evlsi_uart_open,
    .release = evlsi_uart_release,
    .read = evlsi_uart_read,
    .write = evlsi_uart_write,
    .unlocked_ioctl = evlsi_uart_ioctl,
};

static int  evlsi_uart_uevent(const struct device *dev, struct kobj_uevent_env *env)
{
    add_uevent_var(env, "DEVMODE=%#o", 0666);
    return 0;
}

static int __init evlsi_uart_init(void)
{
    //alloc and register char device to the system
    if(alloc_chrdev_region(&dev, 0, 1, "evlsi_uart") < 0)
    {
        pr_info("EVLSI UART Driver Registration Failed\n");
        goto err_alloc_chrdev_region;
    }
    pr_info("EVLSI UART Driver Registered with Major Number: %d and Minor Number: %d\n", MAJOR(dev), MINOR(dev));

    //add char device to the system
    cdev_init(&evlsi_cdev, &fops);
    if(cdev_add(&evlsi_cdev, dev, 1) < 0)
    {
        pr_info("Failed to add evlsi_uart device to the system\n");
        goto err_cdev_add;
    }

    //create a class
    if(IS_ERR(evlsi_device_class = class_create("evlsi_uart_class")))
    {
        pr_info("Failed to create the evlsi_uart class.\n");
        goto err_create_class;
    }
    evlsi_device_class->dev_uevent = evlsi_uart_uevent;

    //create a device
    if(IS_ERR(device_create(evlsi_device_class, NULL, dev, NULL, "evlsi_uart_device")))
    {
        pr_info("Failed to create the evlsi_uart device.\n");
        goto err_device_create;
    }

    //allocate memory for RX buffer
    if((rx_buffer = kmalloc(BUFFER_SIZE, GFP_KERNEL)) == NULL)
    {
        pr_info("Failed to allocate memory for RX buffer\n");
        return -ENOMEM;
    }

    pr_info("EVLSI UART Driver Initialized\n");
    return 0;

err_device_create:
    class_destroy(evlsi_device_class);

err_create_class:
    unregister_chrdev_region(dev, 1);

err_cdev_add:
    cdev_del(&evlsi_cdev);

err_alloc_chrdev_region:
    return EFAULT;
}


static void  __exit evlsi_uart_exit(void)
{
    device_destroy(evlsi_device_class, dev);
    class_destroy(evlsi_device_class);
    unregister_chrdev_region(dev, 1);
    cdev_del(&evlsi_cdev);
    if (rx_buffer)
        kfree(rx_buffer);
    pr_info("EVLSI UART Driver Exit\n");

}

module_init(evlsi_uart_init);
module_exit(evlsi_uart_exit);


MODULE_LICENSE("GPL");
MODULE_AUTHOR("Shivu");
MODULE_DESCRIPTION("EVLSI UART Driver");
MODULE_VERSION("1.0");
