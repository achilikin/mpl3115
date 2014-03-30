MPL3115
=======

MPL3115 Altitude/Pressure Sensor communication example for Raspberry Pi.

Something like this:

    #include "rpi_i2c.h"
    #include "mpl3115.h"

    int main()
    {
        uint8_t buf[32];

	    if (!bcm2835_init())
		    return -1;

        rpi_i2c_open();
        rpi_i2c_config(I2C_SPEED_100K);
        mpl_config();
    
        rpi_i2c_read_regs(MPL3115_STATUS_REG | RPI2C_RS_READ, buf, 6);
        float pressure = mpl_get_pres(&buf[1])/100.0;
	    float temperature = mpl_get_temp(&buf[4]);
	
	    printf("t=%.2f P=%4.fkP\n", temperature, pressure);

	    rpi_i2c_close();
	    bcm2835_close();	
    }
