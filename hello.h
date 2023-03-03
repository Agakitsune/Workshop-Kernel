#include <linux/init.h>
#include <linux/slab.h>
#include <linux/module.h>
#include <linux/kernel.h>
#include <linux/fs.h>

static int __init my_init(void);
static void __exit my_exit(void);

static int my_dev_open(struct inode *inode, struct file *file);
static int my_dev_close(struct inode *inode, struct file *file);
static ssize_t my_dev_read(struct file *file, char *buf, size_t len, loff_t *off);
static ssize_t my_dev_write(struct file *file, const char *buf, size_t len, loff_t *off);
