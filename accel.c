#include <linux/kernel.h>
#include <linux/module.h>
#include <linux/i2c.h>
#include <linux/kobject.h>


enum { DATA_SIZE_MAX = 4 };
struct i2c_adapter* i2c_dev;
struct i2c_client* i2c_client;

static struct i2c_board_info __initdata board_info[] =  {
	{
		I2C_BOARD_INFO("MMA8452Q", 0x1d),
	}
};

static ssize_t x_show(struct kobject *kobj, struct kobj_attribute *attr, char *buff) {
        int x_msb = i2c_smbus_read_byte_data(i2c_client, 0x01);
        int x_lsb = i2c_smbus_read_byte_data(i2c_client, 0x02);
        int x_sign = x_msb >> 7;
        int x = ((x_msb << 4 | x_lsb) ^ (x_sign << 7)) | (x_sign << 31);
        if(x_sign == 1) {
            x = ~x | (x_sign << 31);
        }
        snprintf(buff, 5, "%x\n", x);
        return 5;
}

static ssize_t y_show(struct kobject *kobj, struct kobj_attribute *attr, char *buff) {
        int y_msb = i2c_smbus_read_byte_data(i2c_client, 0x03);
        int y_lsb = i2c_smbus_read_byte_data(i2c_client, 0x04);
        int y_sign = y_msb >> 7;
        int y = ((y_msb << 4 | y_lsb) ^ (y_sign << 7)) | (y_sign << 31);
        if(y_sign == 1) {
            y = ~y | (y_sign << 31);
        } 
        
        snprintf(buff, 5, "%x\n", y);  
        return 5;
}

static ssize_t z_show(struct kobject *kobj, struct kobj_attribute *attr, char *buff) {
        int z_msb = i2c_smbus_read_byte_data(i2c_client, 0x05);
        int z_lsb = i2c_smbus_read_byte_data(i2c_client, 0x06);
        int z_sign = z_msb >> 7;
        int z = ((z_msb << 4 | z_lsb) ^ (z_sign << 7)) | (z_sign << 31);
        if(z_sign == 1) {
            z = ~z | (z_sign << 31);
        }
        snprintf(buff, 5, "%x\n", z);
        return 5;
}

static struct kobj_attribute x_attribute = __ATTR(x, S_IRUGO | S_IWUSR, x_show, NULL);

static struct kobj_attribute y_attribute = __ATTR(y, S_IRUGO | S_IWUSR, y_show, NULL);

static struct kobj_attribute z_attribute = __ATTR(z, S_IRUGO | S_IWUSR, z_show, NULL);

static struct attribute *attrs[] = {
    &x_attribute.attr,
    &y_attribute.attr,
    &z_attribute.attr, 
    //&some_other_attribute.attr, //like this - HINT HINT
    NULL,
};

static struct attribute_group attr_group = {
    .attrs = attrs,
};


static struct kobject *kobj;

static int __init accel_init(void) {
    int ret;
    i2c_dev = i2c_get_adapter(1);
    printk(KERN_DEBUG "accelerometer init\n");

    i2c_client = i2c_new_device(i2c_dev, board_info);
    i2c_smbus_write_byte_data(i2c_client, 0x2A, 0x01);
    

    kobj = kobject_create_and_add("data", kernel_kobj);
    if (!kobj)
        return -ENOMEM;
    ret = sysfs_create_group(kobj, &attr_group);
    if (ret)
        kobject_put(kobj);
    return ret;
}



static void __exit accel_exit(void) {
	printk(KERN_DEBUG "accelerometer exit\n");
	i2c_unregister_device(i2c_client);
    kobject_put(kobj);
}

module_init(accel_init);
module_exit(accel_exit);
MODULE_LICENSE("GPL");