#include <linux/init.h>
#include <linux/module.h>
#include <linux/moduleparam.h>
#include <linux/kernel.h>
#include <linux/proc_fs.h>
#include <linux/uaccess.h>
#include <linux/slab.h>

MODULE_LICENSE("GPL");
MODULE_AUTHOR("Swaraj Hota");
MODULE_DESCRIPTION("A character driver that returns checksum of input");
MODULE_VERSION("0.1");

static uint32_t checksum = 0;

static struct proc_dir_entry *entry;

static ssize_t checksum_write(struct file *file, const char __user *ubuf,
                              size_t count, loff_t *ppos)
{
        char *buf;
        int i;

        printk(KERN_DEBUG "CSUM: write handler called\n");
        if (*ppos > 0)
                return 0;

        buf = kmalloc(count, GFP_KERNEL);
        if (!buf)
                return -ENOMEM;

        if (copy_from_user(buf, ubuf, count)) {
                kfree(buf);
                return -EFAULT;
        }

        checksum = 0;
        for (i = 0; i < count && buf[i] != '\0'; i++)
                checksum += buf[i];

        kfree(buf);
        *ppos = i;
        return i;
}

static ssize_t checksum_read(struct file *file, char __user *ubuf,
                             size_t count, loff_t *ppos)
{
        char *buf;
        int len = 0;

        printk(KERN_DEBUG "CSUM: read handler called\n");
        if (*ppos > 0)
                return 0;

        len = snprintf(NULL, 0, "checksum: %d\n", checksum);
        buf = kmalloc(len + 1, GFP_KERNEL);
        if (!buf)
                return -ENOMEM;

        len = sprintf(buf, "checksum: %d\n", checksum);
        if (copy_to_user(ubuf, buf, len)) {
                kfree(buf);
                return -EFAULT;
        }

        kfree(buf);
        *ppos = len;
        return len;
}

static struct file_operations myops =
{
        .owner = THIS_MODULE,
        .read  = checksum_read,
        .write = checksum_write,
};

static int __init checksum_init(void)
{
        printk(KERN_DEBUG "CSUM: init\n");
        entry = proc_create("csumdev", 0777, NULL, &myops);
        return 0;
}

static void __exit checksum_exit(void)
{
        printk(KERN_DEBUG "CSUM: exit\n");
        proc_remove(entry);
}

module_init(checksum_init);
module_exit(checksum_exit);
