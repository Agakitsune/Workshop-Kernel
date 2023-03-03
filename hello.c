#include "hello.h"

MODULE_LICENSE("MIT");
MODULE_DESCRIPTION("OUR first kernel module");
MODULE_AUTHOR("Agakitsune & deadpanda-c");

long n = 0;
char *str = "ui";

module_param(n, long, S_IRUSR);
module_param(str, charp, S_IRUSR);

int MAJOR = 0;
int BISTRO_MAJOR = 0;

int IS_OPEN = 0;

static char *output = NULL;
static char *bis = NULL;

static struct file_operations op = {
    .open = my_dev_open,
    .release = my_dev_close,
    .read = my_dev_read,
    .write = my_dev_write
};

static struct file_operations bis_op = {
    .open = my_dev_open,
    .release = my_dev_close,
    .read = my_kbistro_read,
    .write = my_kbistro_write
};

int __init my_init(void) {
    

    MAJOR = register_chrdev(0, "furr", &op);
    BISTRO_MAJOR = register_chrdev(0, "bistro", &bis_op);

    printk("CLASSIC: %d\n", MAJOR);
    printk("BISTRO: %d\n", BISTRO_MAJOR);

    // open("/dev/furr", O_RDWR);
    // open("/dev/furr", O_RDWR);

    bis = kmalloc(256, GFP_KERNEL);
    if (bis == NULL)
        return -1;
    bis[0] = '0';

    return 0;
}

void __exit my_exit(void) {
    unregister_chrdev(MAJOR, "furr");
    unregister_chrdev(BISTRO_MAJOR, "bistro");

    printk("Goodbye!\n");
}

int my_dev_open(struct inode *inode, struct file *file) {
    if (IS_OPEN == 0) {
        printk("Device is open\n");
        IS_OPEN = 1;
        return 0;
    } else {
        printk("Device is already opened\n");
        return -1;
    }
}

int my_dev_close(struct inode *inode, struct file *file) {
    if (IS_OPEN == 1) {
        printk("Device is close\n");
        IS_OPEN = 0;
        return 0;
    } else {
        printk("Device is already closed\n");
        return -1;
    }
}

ssize_t my_dev_read(struct file *file, char *buf, size_t len, loff_t *off) {
    unsigned int ui = 0;
    if (buf == NULL)
        return -1;
    output = krealloc(output, len + 1, GFP_KERNEL);
    if (output == NULL)
        return -1;
    // printk("Output is: [%s]\n", output);
    ui = copy_to_user(buf, output, len);
    kfree(output);
    return len - ui;
}

ssize_t my_dev_write(struct file *file, const char *buf, size_t len, loff_t *off) {
    long i = 0;
    unsigned int ui = 0;
    if (buf == NULL)
        return -1;
    output = kmalloc(len * 2 + 1, GFP_KERNEL);
    if (output == NULL)
        return -1;
    ui = copy_from_user(output, buf, len);
    ui = copy_from_user(output + len, buf, len);
    printk("Output is: [%s]\n", output);
    for (; i < len * 2; ++i) {
        if (96 < output[i] && output[i] < 123)
            output[i] -= 32;
    }
    printk("Output is: [%s]\n", output);
    return len - ui;
}

static ssize_t my_kbistro_read(struct file *file, char *buf, size_t len, loff_t *off) {
    unsigned int ui = 0;
     if (buf == NULL)
        return -1;
    if (bis == NULL) {
        bis = kmalloc(len + 1, GFP_KERNEL);
        if (bis == NULL)
            return -1;
        bis[0] = '0';
    } else {
        bis = krealloc(bis, len + 1, GFP_KERNEL);
        if (bis == NULL)
            return -1;
    }
    ui = copy_to_user(buf, bis, len);
    // kfree(bis);
    // bis = NULL;
    return len - ui;
}

static ssize_t my_kbistro_write(struct  file *file, const char *buf, size_t len, loff_t *off) {
    long store = 0;
    long add = 0;
    if (buf == NULL)
        return -1;
    if (bis == NULL) {
        bis = kmalloc(256, GFP_KERNEL);
        if (bis == NULL)
            return -1;
        bis[0] = '0';
    }
    //     bis = krealloc(bis, len + 2, GFP_KERNEL);
    //     if (bis == NULL)
    //         return -1;
    // }
    if (kstrtol(buf, 10, &add) == -1)
        return -1;
    if (kstrtol(bis, 10, &store) == -1)
        return -1;
    sprintf(bis, "%ld", store + add);
    return len;
}


module_init(my_init)
module_exit(my_exit)
