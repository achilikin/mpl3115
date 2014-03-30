/*
	MPL3115 communication wrapper 
	Copyright (C) 2013 Andrey Chilikin https://github.com/achilikin

	This program is free software: you can redistribute it and/or modify
	it under the terms of the GNU General Public License as published by
	the Free Software Foundation, either version 3 of the License, or
	(at your option) any later version.

	This program is distributed in the hope that it will be useful,
	but WITHOUT ANY WARRANTY; without even the implied warranty of
	MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
	GNU General Public License for more details.

	You should have received a copy of the GNU General Public License
	along with this program.  If not, see <http://www.gnu.org/licenses/>.
*/

#ifndef __MPL3115A2_H__
#define __MPL3115A2_H__

#ifdef __cplusplus
extern "C" {
#if 0 // dummy bracket for VAssistX
}
#endif
#endif

#define MPL3115_I2C_ADDRESS 0x60

#define MPL3115_STATUS_REG  0x00

#define MPL3115_P_REG   0x01
#define MPL3115_P_MIN   0x1C
#define MPL3115_P_MAX   0x21

#define MPL3115_T_REG   0x04
#define MPL3115_T_MIN   0x1F
#define MPL3115_T_MAX   0x24

#define MPL3115_ID_REG  0x0C
#define MPL3115A2_ID    0xC4

#define MPL3115_DATA_CFG 0x13 // Sensor data event flag register
#define MPL_CFG_DREM     0x04 // Data Ready Event Mode
#define MPL_CFG_PDEFE    0x02 // Pressure Data Event Flag Enabled
#define MPL_CFG_TDEFE    0x01 // Temperature Data Event Flag Enabled

#define MPL3115_CTRL_REG1  0x26 // Control register #1
#define MPL_CTRL_REG1_SBYB 0x01 // active
#define MPL_CTRL_REG1_OST  0x02 // one shot mode
#define MPL_CTRL_REG1_RST  0x04 // SW Reset
#define MPL_CTRL_REG1_OS0  0x08 // Oversample bit 0
#define MPL_CTRL_REG1_OS1  0x10 // Oversample bit 1
#define MPL_CTRL_REG1_OS2  0x20 // Oversample bit 2
#define MPL_CTRL_REG1_RAW  0x40 // RAW output mode
#define MPL_CTRL_REG1_ALT  0x80 // Altimeter mode

// Oversample Ratio 1
#define MPL_CTRL_OS_1   (0)										                    //  6 ms min between samples
#define MPL_CTRL_OS_2   (MPL_CTRL_REG1_OS0)											//  10 ms
#define MPL_CTRL_OS_4   (MPL_CTRL_REG1_OS1)											//  18 ms
#define MPL_CTRL_OS_8   (MPL_CTRL_REG1_OS1 | MPL_CTRL_REG1_OS0)						//  34 ms
#define MPL_CTRL_OS_16  (MPL_CTRL_REG1_OS2)											//  66 ms
#define MPL_CTRL_OS_32  (MPL_CTRL_REG1_OS2 | MPL_CTRL_REG1_OS0)						// 130 ms
#define MPL_CTRL_OS_64  (MPL_CTRL_REG1_OS2 | MPL_CTRL_REG1_OS1)						// 258 ms
#define MPL_CTRL_OS_128 (MPL_CTRL_REG1_OS2 | MPL_CTRL_REG1_OS1 | MPL_CTRL_REG1_OS0) // 512 ms

static inline int mpl_config(void)
{
	rpi_i2c_set_slave(MPL3115_I2C_ADDRESS);

	int iret = rpi_i2c_read_reg(MPL3115_ID_REG | RPI2C_RS_READ);
	if (iret != MPL3115A2_ID) {
		printf("Invalid device ID: %02X, must be %02X\n", iret, MPL3115A2_ID);
		return -1;
	}

	// set MPL to active state with oversampling ratio 64 (258 ms between samples)
	rpi_i2c_write_reg(MPL3115_CTRL_REG1, MPL_CTRL_OS_64 | MPL_CTRL_REG1_SBYB);
	// enable data ready flags for pressure and temperature (we can use it in future)
	rpi_i2c_write_reg(MPL3115_DATA_CFG, MPL_CFG_DREM | MPL_CFG_PDEFE | MPL_CFG_TDEFE);

	return 0;
}

static inline float mpl_get_temp(uint8_t *buf)
{
	float temp = (int)buf[0];
	if (buf[1] & 0x80) temp += 0.5;
	if (buf[1] & 0x40) temp += 0.25;
	if (buf[1] & 0x20) temp += 0.125;
	if (buf[1] & 0x10) temp += 0.0625;

	return temp;
}

static inline float mpl_get_pres(uint8_t *buf)
{
	float pres;
	uint32_t ipres;

	ipres = ((uint32_t)buf[0]) << 10;
	ipres |= ((uint32_t)buf[1]) << 2;
	ipres |= (uint32_t)buf[2] >> 6;
	pres = ipres;

	if (buf[2] & 0x20) pres += 0.5;
	if (buf[2] & 0x10) pres += 0.25;

	return pres;
}

#ifdef __cplusplus
}
#endif

#endif
