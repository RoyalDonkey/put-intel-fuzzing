/* Simics shared memory module.
 *
 * This module creates a very basic character device at /dev/simics_shm.
 * This device is mapped to a fixed memory region, which is assumed to just
 * exist for the sole purpose of this device (e.g. defined in device tree).
 * Simics also knows about the region, thus it is possible for a Linux process
 * to communicate with Simics by writing into the /dev/simics_shm device and
 * then emitting a MAGIC instruction, which prompts Simics to read.
 *
 * Warning: This module is not thread-safe. If there is a need for multiple
 * writers, perhaps multiple devices (/dev/simics_shm1, /dev/simics_shm2, ...)
 * would be the cleanest solution, but it is also possible to extend a single
 * device with a mutex or similar.
 * */
#include "linux/slab.h"
#include "linux/uaccess.h"
#include <linux/module.h>
#include <linux/init.h>
#include <linux/platform_device.h>
#include <linux/fs.h>
#include <linux/cdev.h>
#include <linux/io.h>
#include <asm/uaccess.h>

/* Maximum number of devices (minor versions) */
#define MAX_DEVICES 1

/* The physical memory region shared with Simics */
#define SIMICS_SHM_ADDR 0x20000000
#define SIMICS_SHM_SIZE 0x1000

struct my_device_data {
	struct cdev cdev;
	/* arbitrary device data can be stored here */
	size_t write_pos;
	size_t read_pos;
	char is_continuation;
};

static int my_driver_probe(struct platform_device *pdev);
static void my_driver_remove(struct platform_device *pdev);
static int my_open(struct inode *inode, struct file *file);
static int my_release(struct inode *inode, struct file *file);
static ssize_t my_read(struct file *file, char __user *buf, size_t count, loff_t *offset);
static ssize_t my_write(struct file *file, const char __user *buf, size_t count, loff_t *offset);
static int __init my_init(void);
static void __exit my_exit(void);

static struct platform_driver my_driver = {
	.driver = {
		.name = "simics_shm",
	},
	.probe = my_driver_probe,
	.remove = my_driver_remove,
};

static struct platform_device my_dev = {
	.name = "simics_shm",
	.id = -1,
	.driver_override="simics_shm"
};

static struct my_device_data dev_data[MAX_DEVICES];
static int dev_major;
static struct class *my_class;
static void __iomem *mapped_memory;

static const struct file_operations my_fops = {
	.owner = THIS_MODULE,
	.read  = my_read,
	.write = my_write,
	.open  = my_open,
	.release = my_release,
};

static int my_driver_probe(struct platform_device *pdev)
{
	int err;
	dev_t dev;

	err = alloc_chrdev_region(&dev, 0, MAX_DEVICES, "simics_shm");
	if (err != 0) {
		return err;
	}
	dev_major = MAJOR(dev);
	my_class = class_create("intel_simics");

	cdev_init(&dev_data[0].cdev, &my_fops);
	cdev_add(&dev_data[0].cdev, dev, 1);
	device_create(my_class, NULL, dev, NULL, "simics_shm");

	// Map the physical memory region
	mapped_memory = ioremap(SIMICS_SHM_ADDR, SIMICS_SHM_SIZE);
	if (!mapped_memory) {
		printk(KERN_ERR"simics_shm: Failed to map memory!\n");
		return -ENOMEM;
	}

	dev_data[0].write_pos = 0;
	dev_data[0].read_pos = 0;
	dev_data[0].is_continuation = 0;

	return 0;
}

static void my_driver_remove(struct platform_device *pdev)
{
	iounmap(mapped_memory);
	cdev_del(&dev_data[0].cdev);
	unregister_chrdev_region(MKDEV(dev_major, 0), MAX_DEVICES);
	class_destroy(my_class);
	dev_data[0].write_pos = 0;
	dev_data[0].read_pos = 0;
	dev_data[0].is_continuation = 0;
}

static int my_open(struct inode *inode, struct file *file)
{
	printk(KERN_ERR"simics_shm: Device open!\n");
	struct my_device_data *my_data = container_of(inode->i_cdev, struct my_device_data, cdev);
	file->private_data = my_data;
	dev_data[0].is_continuation = 0;
	return 0;
}

static int my_release(struct inode *inode, struct file *file)
{
	printk(KERN_ERR"simics_shm: Device close!\n");
	return 0;
}

static ssize_t my_read(struct file *file, char __user *buf, size_t count, loff_t *offset)
{
	const size_t bytes_left = dev_data[0].write_pos - dev_data[0].read_pos;
	if (dev_data[0].read_pos > dev_data[0].write_pos) {
		printk(KERN_ERR"simics_shm: Internal error (read_pos > write_pos)!\n");
		return -EINVAL;
	}
	printk(KERN_ERR"simics_shm: Device read! (offset=%u)\n", offset);
	if (bytes_left == 0) {
		dev_data[0].read_pos = 0;
		return 0;
	}
	if (bytes_left < count) {
		count = bytes_left;
	}
	if (copy_to_user(buf, ((char*)mapped_memory) + dev_data[0].read_pos, count)) {
		return -EFAULT;
	}
	dev_data[0].read_pos += count;
	return count;
}

static ssize_t my_write(struct file *file, const char __user *buf, size_t count, loff_t *offset)
{
	printk(KERN_ERR"simics_shm: Device write! (count=%zu, offset=%u)\n", count, offset);
	if (dev_data[0].write_pos >= SIMICS_SHM_SIZE) {
		printk(KERN_ERR"simics_shm: Internal error (write_pos >= SIMICS_SHM_SIZE)!\n");
		return -EINVAL;
	}
	printk(KERN_ERR"simics_shm: write bp1\n", count, offset);
	if (!dev_data[0].is_continuation) {
		/* Not a continuation; overwrite from the start of the buffer */
		dev_data[0].write_pos = 0;
		dev_data[0].read_pos = 0;
	}
	printk(KERN_ERR"simics_shm: write bp2\n", count, offset);
	if (count > SIMICS_SHM_SIZE - dev_data[0].write_pos) {
		printk(KERN_ERR"simics_shm: Out of memory (SIMICS_SHM_SIZE too small)!\n");
		return -EINVAL;
	}
	printk(KERN_ERR"simics_shm: write bp3\n", count, offset);
	if (copy_from_user(((char*)mapped_memory) + dev_data[0].write_pos, buf, count)) {
		return -EFAULT;
	}
	printk(KERN_ERR"simics_shm: write bp4\n", count, offset);
	dev_data[0].write_pos += count;
	dev_data[0].is_continuation = 1;
	return count;
}

static int __init my_init(void)
{
	printk(KERN_ERR"simics_shm: Registering device driver!\n");
	platform_device_register(&my_dev);
	return platform_driver_register(&my_driver);
}

static void __exit my_exit(void)
{
	printk(KERN_ERR"simics_shm: Unregistering the driver!\n");
	platform_driver_unregister(&my_driver);
}

module_init(my_init);
module_exit(my_exit);
MODULE_AUTHOR("Piotr Kaszubski");
MODULE_DESCRIPTION("Simics shared memory device");
MODULE_LICENSE("Dual MIT/GPL");
