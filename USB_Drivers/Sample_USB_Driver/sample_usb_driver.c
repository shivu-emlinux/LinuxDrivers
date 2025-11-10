#include <linux/kernel.h>
#include <linux/module.h>
#include <linux/usb.h>


#define VENDOR_ID 0x14cd
#define PRODUCT_ID 0x125d

static struct usb_device *usb_device;
static struct usb_class_driver usb_class;
static unsigned char blk_in_buffer[512];

static int usb_open(struct inode *inode, struct file *file)
{
    return 0;

}

static  int usb_release(struct inode *indoe, struct file *file)
{
    return 0;

}

static ssize_t usb_read(struct file *file, char __user *buffer, size_t length, loff_t *offset)
{
    int retval;
    int read_count =0;

    pr_info("Read function called\n");

    retval = usb_bulk_msg(usb_device, usb_rcvbulkpipe(usb_device, 0x82), blk_in_buffer, length, &read_count, 5000);
    if(retval){
        pr_info("Error in bulk read: %d\n", retval);
        return retval;
    }
    if(copy_to_user(buffer, blk_in_buffer, read_count)){
        pr_info("Error in copy to user\n");
        return -EFAULT;
    }

    return(ssize_t)read_count;
    return 0;
    
}

static ssize_t usb_write(struct file *file, const char __user *buffer, size_t length, loff_t *offset)
{
    pr_info("Write function called\n");
    int retval;
    int wrote_count = 512;
    if(copy_from_user(blk_in_buffer, buffer, length)){
        pr_info("Error in copy from user\n");
        return -EFAULT;
    }
    retval = usb_bulk_msg(usb_device, usb_sndbulkpipe(usb_device, 0x01), blk_in_buffer, length, &wrote_count, 5000);
    if(retval){
        pr_info("Error in bulk write: %d\n", retval);
        return retval;
    }
    return wrote_count;
}




static struct file_operations fops = {
    .owner = THIS_MODULE,
    .open = usb_open,
    .release = usb_release,
    .read = usb_read,
    .write = usb_write,
};


struct usb_device_id sample_usb_table[] = {
    { USB_DEVICE(VENDOR_ID, PRODUCT_ID) },
    {},
};

MODULE_DEVICE_TABLE(usb_device_id, sample_usb_table);

static void print_endpoint_info(struct usb_interface *interface)
{
    struct usb_host_interface *iface_desc;
    struct usb_endpoint_descriptor *endpoint;
    int i;

    iface_desc = interface->cur_altsetting;

    pr_info("Number of endpoints: %d\n", iface_desc->desc.bNumEndpoints);

    for (i = 0; i < iface_desc->desc.bNumEndpoints; i++) {
        endpoint = &iface_desc->endpoint[i].desc;

        pr_info("Endpoint %d:\n", i);
        pr_info("  Address: 0x%02x\n", endpoint->bEndpointAddress);
        pr_info("  Attributes: 0x%02x\n", endpoint->bmAttributes);
        pr_info("  Max Packet Size: %d\n", usb_endpoint_maxp(endpoint));
        pr_info("  Interval: %d\n", endpoint->bInterval);
    }
}

static int myusb_probe(struct usb_interface *interface, const struct usb_device_id *id)
{
    int retval;
    pr_info("Sample USB Device: No of Alternate Settings = %d\n", interface->num_altsetting);
    print_endpoint_info(interface);
    pr_info("Sample USB Device Connected: VID=0x%04x, PID=0x%04x\n", id->idVendor, id->idProduct);
    pr_info("Sample USB Device: %d\n", interface->usb_dev);

    usb_device = interface_to_usbdev(interface);
    usb_class.name = "usb/sample_usb%d";
    usb_class.fops = &fops;

    if( (retval = usb_register_dev(interface, &usb_class)) < 0){
        pr_info("Failed to register the device\n");
    }else
    {
        pr_info("Device registered with minor number %d\n", interface->minor);
    }

    return retval;
}

static void myusb_disconnect(struct usb_interface *interface)
{
    usb_deregister_dev(interface, &usb_class);
    pr_info("Sample USB is  disconnected\n");
}


struct usb_driver sample_usb_driver = {
    .name = "sample_usb_driver",
    .id_table = sample_usb_table,
    .probe = myusb_probe,
    .disconnect = myusb_disconnect,
};

static int __init sample_usb_driver_init(void)
{
    int result;
    result =  usb_register(&sample_usb_driver); //USB core API
    if(result < 0){
        pr_info("Failed to register Sample USB Driver\n");
    }
        pr_info("Sample USB Driver Initialized\n");
    return 0;

}

static void __exit sample_usb_driver_exit(void)
{
    pr_info("Sample USB Driver Exited\n");
    usb_deregister(&sample_usb_driver); //USB core API 
}

module_init(sample_usb_driver_init);
module_exit(sample_usb_driver_exit);

MODULE_LICENSE("GPL");
MODULE_AUTHOR("Shivu");
MODULE_DESCRIPTION("A sample driver for a USB device");