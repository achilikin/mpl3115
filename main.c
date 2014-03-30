/*
 * MPL3115A2 Altitude/Pressure Sensor Sparkfun breakout
 * reading example for Raspberry Pi.
 * Requires bcm2835 library http://www.airspayce.com/mikem/bcm2835/
 */

#include <stdio.h>
#include <string.h>

#include "rpi_i2c.h"
#include "mpl3115.h"

int main()
{
	uint8_t buf[32];

	if (!bcm2835_init())
		return -1;

	rpi_i2c_open();
	rpi_i2c_config(I2C_SPEED_100K);
	if (mpl_config() != 0)
		return -1;

	memset(buf, 0, sizeof(buf));
	if (rpi_i2c_read_regs(MPL3115_STATUS_REG | RPI2C_RS_READ, buf, 6) < 0) {
		printf("Unable to read MPL3115!\n");
		return -1;
	}

	float pressure = mpl_get_pres(&buf[1])/100.0;
	float temperature = mpl_get_temp(&buf[4]);

	printf("t=%.2f P=%4.fkP\n", temperature, pressure);

	rpi_i2c_close();
	bcm2835_close();	
}
