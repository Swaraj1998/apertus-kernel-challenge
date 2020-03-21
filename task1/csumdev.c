#include <linux/init.h>
#include <linux/module.h>
#include <linux/moduleparam.h>
#include <linux/kernel.h>
#include <linux/proc_fs.h>
#include <linux/uaccess.h>
#include <linux/slab.h>
#include <linux/device.h>
#include <linux/fs.h>
#include <linux/err.h>

#include "csumdev.h"

#define DEVICE_NAME "csumdev"
#define CLASS_NAME  "csum"

MODULE_LICENSE("GPL");
MODULE_AUTHOR("Swaraj Hota");
MODULE_DESCRIPTION("A character driver that returns checksum of input");
MODULE_VERSION("0.1");

static int dev_major = 0;
module_param(dev_major, int, 0664);

static int dev_minor = 0;
module_param(dev_minor, int, 0664);

static struct class  *checksum_class = NULL;
static struct device *checksum_dev   = NULL;

static struct proc_dir_entry *proc_entry = NULL;

static uint32_t checksum = 0;

static ssize_t checksum_write(struct file *file, const char __user *ubuf,
                              size_t count, loff_t *ppos)
{
        unsigned char *buf;
        unsigned i;

        printk(KERN_DEBUG "CSUM: write handler called\n");

        buf = kmalloc(count, GFP_KERNEL);
        if (!buf)
                return -ENOMEM;

        if (copy_from_user(buf, ubuf, count)) {
                kfree(buf);
                return -EFAULT;
        }

        if (*ppos == 0)
                checksum = 0;

        for (i = 0; i < count; i++)
                checksum += buf[i];

        kfree(buf);
        *ppos += i;
        return i;
}

static ssize_t checksum_read(struct file *file, char __user *ubuf,
                             size_t count, loff_t *ppos)
{
        char *buf;
        unsigned len = 0;

        printk(KERN_DEBUG "CSUM: read handler called\n");
        if (*ppos > 0)
                return 0;

        len = snprintf(NULL, 0, "checksum: %u\n", checksum);
        if (count < len + 1)
                return 0;
        buf = kmalloc(len + 1, GFP_KERNEL);
        if (!buf)
                return -ENOMEM;

        len = sprintf(buf, "checksum: %u\n", checksum);
        if (copy_to_user(ubuf, buf, len)) {
                kfree(buf);
                return -EFAULT;
        }

        kfree(buf);
        *ppos = len;
        return len;
}

static long checksum_ioctl(struct file *file, unsigned int cmd,
                           unsigned long arg)
{
        struct csum_arg_t ret;
        unsigned len;

        if (_IOC_TYPE(cmd) != CSUM_IOC_MAGIC)
                return -ENOTTY;

        switch (cmd) {
        case CSUM_GET_STRING:
                len = sprintf(ret.str, "checksum: %u\n", checksum);
                if (copy_to_user((struct csum_arg_t __user *)arg, ret.str, len))
                        return -EFAULT;
                break;
        default:
                return -ENOTTY;
        }

        return 0;
}

static struct file_operations f_ops =
{
        .owner          = THIS_MODULE,
        .read           = checksum_read,
        .write          = checksum_write,
        .unlocked_ioctl = checksum_ioctl
};

static int __init checksum_init(void)
{
        int ret;

        printk(KERN_DEBUG "CSUM: init\n");

        ret = register_chrdev(dev_major, DEVICE_NAME, &f_ops);
        if (ret < 0) {
                printk(KERN_WARNING "CSUM: can't register device major");
                return ret;
        }

        if (!dev_major)
                dev_major = ret;

        checksum_class = class_create(THIS_MODULE, CLASS_NAME);
        if (IS_ERR(checksum_class)) {
                printk(KERN_WARNING "CSUM: can't register device class");
                ret = PTR_ERR(checksum_class);
                goto out_class;
        }

        checksum_dev = device_create(checksum_class, NULL,
                                MKDEV(dev_major, dev_minor), NULL, DEVICE_NAME);
        if (IS_ERR(checksum_dev)) {
                printk(KERN_WARNING "CSUM: can't create the device");
                ret = PTR_ERR(checksum_dev);
                goto out_dev;
        }

        proc_entry = proc_create(DEVICE_NAME, 0664, NULL, &f_ops);
        if (!proc_entry) {
                ret = -ENOMEM;
                goto out_proc;
        }

        ret = 0;
        goto end;

out_proc:
        device_destroy(checksum_class, MKDEV(dev_major, dev_minor));
        class_unregister(checksum_class);
out_dev:
        class_destroy(checksum_class);
out_class:
        unregister_chrdev(dev_major, DEVICE_NAME);
end:
        return ret;
}

static void __exit checksum_exit(void)
{
        printk(KERN_DEBUG "CSUM: exit\n");

        device_destroy(checksum_class, MKDEV(dev_major, dev_minor));
        class_unregister(checksum_class);
        class_destroy(checksum_class);
        unregister_chrdev(dev_major, DEVICE_NAME);

        proc_remove(proc_entry);
}

module_init(checksum_init);
module_exit(checksum_exit);
