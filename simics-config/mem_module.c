#include <linux/module.h>
#include <linux/kernel.h>
#include <linux/of.h>
#include <linux/platform_device.h>
#include <linux/io.h>
#include <linux/kobject.h>
#include <linux/sysfs.h>

#define mem_dev_ADDR 0x20000000

static void __iomem* io_reg = NULL;

static const struct of_device_id deviceSearch[] =
{
	{ .compatible = "mem_dev,my_memory" },
	{ }
};
MODULE_DEVICE_TABLE(of, deviceSearch);

static ssize_t readKObjectAttribute(struct kobject* kobj, struct kobj_attribute* attr, char* buf)
{
	int rtrn_val = 0;

	pr_debug("[readKObjectAttribute] Trying to read from a physical register \"%s\"\n", attr->attr.name);

	if (!strcmp(attr->attr.name, "mem"))
		memcpy_fromio(&rtrn_val, io_reg, 1000);	
	else 
	{
		pr_info("[readKObjectAttribute] Couldn't find Attribute \"%s\"\n", attr->attr.name);
		return -22;	
	}
	pr_info("[readKObjectAttribute] Successful read \"%s\" from \"/sysfs/kernel/%s\" with value: %d\n", attr->attr.name, attr->attr.name, rtrn_val);

	return sprintf(buf, "%d\n", rtrn_val);
}

static ssize_t writeKObjectAttribute(struct kobject* kobj, struct kobj_attribute* attr, const char* buf, size_t count)
{
	int rtrn_val;
	sscanf(buf, "%d", &rtrn_val);

	pr_info("[writeKObjectAttribute] Trying to write to a physical register \"%s\"\n", attr->attr.name);
	if (!strcmp(attr->attr.name, "mem"))
		memcpy_toio(io_reg, &rtrn_val, 1000);
	else 
	{
		pr_info("[writeKObjectAttribute] Couldn't write to Attribute \"%s\"\n", attr->attr.name);
		return -22;	
	}
	pr_info("[writeKObjectAttribute] Successful wrote to \"%s\" in \"/sysfs/kernel/%s\" with value: %d\n", attr->attr.name, attr->attr.name, (strcmp(attr->attr.name, "operation") ? rtrn_val : buf[0]));

	return count;
}

static struct kobject* memDeviceKObject;
static struct kobj_attribute memAttribute = __ATTR(mem,      0660, readKObjectAttribute, writeKObjectAttribute);

static int deviceProbe(struct platform_device* pdev)
{
	struct resource* res;
	void __iomem* baseAddr;

	res = platform_get_resource(pdev, IORESOURCE_MEM, 0);
	if (!res)
	{
		dev_err(&pdev->dev, "[deviceRegister] No memory resource?.\n");
        	return -7;
   	}

	baseAddr = devm_ioremap_resource(&pdev->dev, res);
	if (IS_ERR(baseAddr))
	{
		dev_err(&pdev->dev, "[deviceRegister] Something wrong with memory allocation at 0x%08llx.\n", (unsigned long long)(baseAddr));
        	return -8;
	}

	dev_info(&pdev->dev, "[deviceRegister] Device mapped: 0x%08llx:0x%08llx\n", (unsigned long long)(res->start), (unsigned long long)(res->end));
	return 0;
}

static struct platform_driver deviceDriver = 
{
	.driver = 
	{
		.name = "mem_dev",
		.of_match_table = of_match_ptr(deviceSearch),
	},
	.probe = deviceProbe
};

static int __init my_init(void)
{
	if (platform_driver_register(&deviceDriver))
	{
		pr_crit("Failed to register device driver \"mem_dev\"\n");
		return -7;
	}
	
	io_reg = ioremap(mem_dev_ADDR, 0x10);
	
	if (io_reg == NULL)
	{
		platform_driver_unregister(&deviceDriver);
		pr_crit("Could not map io memory!\n");
		return -9;
	}

	memDeviceKObject = kobject_create_and_add("mem_dev", kernel_kobj);
	if (!memDeviceKObject)
	{
		pr_crit("Could not create a KObject!\n");
		return -12;
	}

	if (sysfs_create_file(memDeviceKObject, &memAttribute.attr))
	{
		pr_crit("Could not create a KObject Attribute \"mem\"!\n");
		return -13;
	}


	pr_info("Added \"mem_dev\" successfully\n");
	
	return 0;
}

static void __exit my_exit(void) 
{
	sysfs_remove_file(memDeviceKObject, &memAttribute.attr);
	kobject_put(memDeviceKObject);

	iounmap(io_reg);
	platform_driver_unregister(&deviceDriver);
	pr_info("Removed \"mem_dev\" successfully\n");
}
module_init(my_init);
module_exit(my_exit);
MODULE_AUTHOR("XXX");
MODULE_DESCRIPTION("Simple I/O buffer");
MODULE_LICENSE("GPL");
