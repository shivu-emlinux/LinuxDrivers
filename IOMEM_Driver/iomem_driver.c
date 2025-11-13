#include <linux/kernel.h>
#include <linux/module.h>
#include <linux/init.h>
#include <linux/device.h>
#include <linux/ioport.h>
#include <asm/io.h>
#include <linux/fs.h>
#include <linux/types.h>
#include <linux/cdev.h>

static dev_t iomem_dev_num;
#define DEVICE_NAME "iomem_driver"
static struct cdev iomem_cdev;
static struct class *iomem_device_class;
#define CLASS_NAME "iomem_class"

#define PERIPHERAL_PHY_ADDRESS 0xfec01000
#define MEM_SIZE 0x0A
static int __iomem *my_virtual_address;


static int iomem_fileop_open(struct inode *inode, struct file *filep);
static int iomem_fileop_release(struct inode *inode, struct file *filep);
static ssize_t iomem_fileop_read(struct file *filep, char __user *userdata, size_t count, loff_t *offset);
static ssize_t iomem_fileop_write(struct file *filep, const char __user *userdata, size_t count, loff_t *offset);

static struct file_operations fops = {
    .owner = THIS_MODULE,
    .open = iomem_fileop_open,
    .release = iomem_fileop_release,
    .read = iomem_fileop_read,
    .write = iomem_fileop_write,
};

static int iomem_fileop_open(struct inode *inode, struct file *filep)
{
    pr_info("%s: Opening...\n", THIS_MODULE->name);
    return 0;
}

static int iomem_fileop_release(struct inode *inode, struct file *filep)
{
    pr_info("%s: Releasing...\n", THIS_MODULE->name);
    return 0;
}

static ssize_t iomem_fileop_read(struct file *filep, char __user *userdata, size_t count, loff_t *offset)
{
    pr_info("%s: Reading...\n", THIS_MODULE->name);
    return 0;
}

static ssize_t iomem_fileop_write(struct file *filep, const char __user *userdata, size_t count, loff_t *offset)
{
    pr_info("%s: Writing...\n", THIS_MODULE->name);
    return 0;
}


static int __init iomem_driver_init(void)
{
    pr_info("%s: Initializing iomem_driver....\n", THIS_MODULE->name);
    if(request_mem_region(PERIPHERAL_PHY_ADDRESS, MEM_SIZE, DEVICE_NAME) == NULL)
    {
        pr_err("%s: Memory request failed\n", THIS_MODULE->name);
        goto req_mem_region_error;
    }
    my_virtual_address = ioremap(PERIPHERAL_PHY_ADDRESS, MEM_SIZE);
    if(!my_virtual_address)
    {
        pr_err("%s: Error in ioremap()\n", THIS_MODULE->name);
        goto ioremap_error;
    }
    
    if(alloc_chrdev_region(&iomem_dev_num, 0, 1, DEVICE_NAME) < 0 )
    {
        pr_err("%s: Error in allocating memory region for this character device\n", THIS_MODULE->name);
        goto alloc_chrdev_region_error;
    }
    cdev_init(&iomem_cdev, &fops);
    if(cdev_add(&iomem_cdev, iomem_dev_num, 1) < 0)
    {
        pr_err("%s: Error while adding chrdev iomem_cdev\n", THIS_MODULE->name);
        goto cdev_add_error;
    }
    if(IS_ERR(iomem_device_class = class_create(CLASS_NAME)))
    {
        pr_err("%s: Error in creating class\n", THIS_MODULE->name);
        goto class_create_error;
    }
    if(IS_ERR(device_create(iomem_device_class, NULL, iomem_dev_num, NULL, DEVICE_NAME)))
    {
        pr_err("%s: Error in creting device\n", THIS_MODULE->name);
        goto device_create_error;
    }
    pr_info("my_virtual_address= %x\n",my_virtual_address);
    //writel(0xCDEF, my_virtual_address);

    //pr_info("==%c\n", readl(my_virtual_address));

    return 0;

device_create_error:
    class_destroy(iomem_device_class);
class_create_error:
    cdev_del(&iomem_cdev);
cdev_add_error:
    unregister_chrdev_region(iomem_dev_num, 1);
alloc_chrdev_region_error:
    iounmap(my_virtual_address);
ioremap_error:
    release_mem_region(PERIPHERAL_PHY_ADDRESS, MEM_SIZE);
req_mem_region_error:
    return EFAULT;
}

static void __exit iomem_driver_exit(void)
{
    device_destroy(iomem_device_class, iomem_dev_num);
    class_destroy(iomem_device_class);
    cdev_del(&iomem_cdev);
    unregister_chrdev_region(iomem_dev_num, 1);
    iounmap(my_virtual_address);
    release_mem_region(PERIPHERAL_PHY_ADDRESS, MEM_SIZE);
    pr_info("%s: Exiting iomem_driver...\n", THIS_MODULE->name);
}

module_init(iomem_driver_init);
module_exit(iomem_driver_exit);

MODULE_LICENSE("GPL");
MODULE_AUTHOR("Shivu");
MODULE_DESCRIPTION("Sample IOMEM driver...");