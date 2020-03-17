#include <linux/init.h>
#include <linux/module.h>
#include <linux/moduleparam.h>
#include <linux/kernel.h>
#include <linux/proc_fs.h>
#include <linux/uaccess.h>

#define BUFSIZE 100

MODULE_LICENSE("GPL");
MODULE_AUTHOR("Swaraj Hota");
MODULE_DESCRIPTION("A character driver that returns checksum of input");
MODULE_VERSION("0.1");

static size_t checksum = 0;

static struct proc_dir_entry *entry;

static ssize_t checksum_write(struct file *file, const char __user *ubuf, 
                              size_t count, loff_t *ppos)
{
        char buf[BUFSIZE];
        int i;
        printk(KERN_DEBUG "CSUM: write handler called\n");
        if (*ppos > 0 || count > BUFSIZE)
                return -EFAULT;

        if (copy_from_user(buf, ubuf, count))
                return -EFAULT;
    
        checksum = 0;
        for (i = 0; i < count && buf[i] != '\0'; i++)
            checksum += buf[i];

        *ppos = i;
        return i;
}

static ssize_t checksum_read(struct file *file, char __user *ubuf,
                             size_t count, loff_t *ppos)
{
        char buf[BUFSIZE];
        int len = 0;
        printk(KERN_DEBUG "CSUM: read handler called\n");
        if (*ppos > 0 || count < BUFSIZE)
                return 0;

        len = sprintf(buf, "checksum: %ld\n", checksum);

        if (copy_to_user(ubuf, buf, len))
                return -EFAULT;

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
