#include <linux/kernel.h>
#include <linux/module.h>
#include <linux/spi/spi.h>
#include "linux/init.h"


#define DRIVER_NAME "evlsi_spi"

static int __init evlsi_spi_init(void)
{
    pr_info("%s: SPI driver initialized\n", DRIVER_NAME);
    return 0;
}

static void __exit evlsi_spi_exit(void)
{
    pr_info("%s: SPI driver exited\n", DRIVER_NAME);
}

module_init(evlsi_spi_init);
module_exit(evlsi_spi_exit);


MODULE_LICENSE("GPL");
MODULE_AUTHOR("Shivu");
MODULE_DESCRIPTION("EVLSI SPI Driver");