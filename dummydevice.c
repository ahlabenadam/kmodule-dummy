/**
 * @file   dummydevice.c
 * @author Aleksey Senin
 * @date   Apr 2024
 * @version 1.0
 * @brief   a dummy device device
 * @see  https://github.com/ahlabenadam/kmodule-dummy
 */

#include <linux/init.h>
#include <linux/module.h>
#include <linux/kernel.h>
#include <linux/fs.h>
#include <linux/uaccess.h>
#include <linux/device.h>
#include <linux/kdev_t.h>
#include <linux/cdev.h>

#define DEVICE_NAME "dummydevice"

MODULE_LICENSE("GPL");
MODULE_AUTHOR("Aleksey Senin");
MODULE_DESCRIPTION("DummyDevice character device Module");
MODULE_VERSION("1.0");            ///< A version number to inform users


static int dev_open(struct inode*, struct file*);
static int dev_release(struct inode*, struct file*);
static ssize_t dev_read(struct file*, char*, size_t, loff_t*);
static ssize_t dev_write(struct file*, const char*, size_t, loff_t*);

static struct file_operations fops = {
    .owner = THIS_MODULE,
    .open = dev_open,
    .read = dev_read,
    .write = dev_write,
    .release = dev_release,
};

struct dmodule {
    struct class *cl;
    struct cdev c_dev;
    dev_t major;
};

static struct dmodule dmod;

static int __init dummydevice_init(void) {
    int ret;

    if ((ret = alloc_chrdev_region(&dmod.major, 0, 1, "dummyregion")) < 0 )
        return ret;

    if ((dmod.cl  = class_create("dummychr")) == NULL) {
        printk(KERN_ALERT "Class create failed\n");
        goto l_chrdev;
    }

    if (device_create(dmod.cl, NULL, dmod.major, NULL, "dummydev") == NULL) {
        printk(KERN_ALERT "device create failed\n");
        goto l_class;
    }

    cdev_init(&dmod.c_dev, &fops);

    if (cdev_add(&dmod.c_dev, dmod.major, 1) == -1 ) {
        printk(KERN_ALERT "adding device failed\n");
        goto l_cdev_add;
    }
    return 0;

l_cdev_add:
    device_destroy(dmod.cl, dmod.major);
l_class:
    class_destroy(dmod.cl);
l_chrdev:
    unregister_chrdev_region(dmod.major, 1);
    return -ENOMEM;
}

static void __exit dummydevice_exit(void) {
    cdev_del(&dmod.c_dev);
    device_destroy(dmod.cl, dmod.major);
    class_destroy(dmod.cl);
    unregister_chrdev_region(dmod.major, 1);
    printk(KERN_INFO "dummydevice module has been unloaded\n");
}

static int dev_open(struct inode *inodep, struct file *filep) {
   printk(KERN_INFO "dummydevice device opened\n");
   return 0;
}

static ssize_t dev_write(struct file *filep, const char *buffer,
                         size_t len, loff_t *offset) {

   printk(KERN_INFO "Sorry, dummydevice is read only\n");
   return -EFAULT;
}

static int dev_release(struct inode *inodep, struct file *filep) {
   printk(KERN_INFO "dummydevice device closed\n");
   return 0;
}

static ssize_t dev_read(struct file *filep, char *buffer, size_t len, loff_t *offset) {
    char lbuf[128];
    int l;

    l = snprintf(lbuf, sizeof(lbuf), "dummymodule read response\n");
    return simple_read_from_buffer(buffer, len, offset, lbuf, l);
}

module_init(dummydevice_init);
module_exit(dummydevice_exit);
