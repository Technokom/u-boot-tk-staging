
#include <common.h>
#include <command.h>
#include <config.h>
#include <environment.h>
#include <i2c.h>
#include <u-boot/rsa.h>
#include <image.h>

#ifndef	CONFIG_SYS_I2C_SPEED
#define	CONFIG_SYS_I2C_SPEED	50000
#endif

static int do_tkserial(cmd_tbl_t *cmdtp, int flag, int argc, char * const argv[])
{
	/* I2C EEPROM */
    int rcode = -1;
    unsigned char buffer[128];
    unsigned short version;
    unsigned long long serial = -1;
	struct udevice *bus;
	struct udevice *dev;
	int ret;


    struct image_region region[1];
    memset(buffer, 0xFF, sizeof(buffer));

    
    do {
	    ret = uclass_get_device_by_seq(UCLASS_I2C, 0, &bus);
        if (ret != 0) break;
        ret = dm_i2c_set_bus_speed(bus, CONFIG_SYS_I2C_SPEED);
        if (ret != 0) break;
        ret = i2c_get_chip(bus, 0x57, 1, &dev);
        if (ret != 0) break;
        rcode = dm_i2c_read(dev, 0, buffer, sizeof(buffer));        

    } while(0);

    if (0 == rcode) {
        version = *((unsigned short*)buffer);
        if (1 == version) {
            serial = *(unsigned long long*)(buffer + 2);
            env_set_ulong ("board_serial", serial);
            /*region[0].data = buffer;
            region[0].size = 30;            
            int result_verify = rsa_verify(NULL,region,1,buffer+30,64);
            
            if (0 == result_verify) {
                puts("sign success\n");
            } else {
                puts("sign fail\n");
            }*/
        } else {
            puts("unsupported version\n");
        }

    } else {
        puts("error i2c read\n");
    }
    

    return rcode;
}

U_BOOT_CMD(tkserial, 1, 1, do_tkserial, "", "");
