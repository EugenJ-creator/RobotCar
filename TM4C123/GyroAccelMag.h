
#ifndef GYRO_ACCEL_MAG_H
#define GYRO_ACCEL_MAG_H 1



	#include <stdint.h>

	typedef int bool;
	#define TRUE  1
	#define FALSE 0


	
	



  enum DlpfBandwidth  {
    DLPF_BANDWIDTH_184HZ = 0x01,
    DLPF_BANDWIDTH_92HZ = 0x02,
    DLPF_BANDWIDTH_41HZ = 0x03,
    DLPF_BANDWIDTH_20HZ = 0x04,
    DLPF_BANDWIDTH_10HZ = 0x05,
    DLPF_BANDWIDTH_5HZ = 0x06
  };
  enum AccelRange {
    ACCEL_RANGE_2G = 0x00,
    ACCEL_RANGE_4G = 0x08,
    ACCEL_RANGE_8G = 0x10,
    ACCEL_RANGE_16G = 0x18
  };
  enum GyroRange {
    GYRO_RANGE_250DPS = 0x00,
    GYRO_RANGE_500DPS = 0x08,
    GYRO_RANGE_1000DPS = 0x10,
    GYRO_RANGE_2000DPS = 0x18
  };
  enum WomRate  {
    WOM_RATE_0_24HZ = 0x00,
    WOM_RATE_0_49HZ = 0x01,
    WOM_RATE_0_98HZ = 0x02,
    WOM_RATE_1_95HZ = 0x03,
    WOM_RATE_3_91HZ = 0x04,
    WOM_RATE_7_81HZ = 0x05,
    WOM_RATE_15_63HZ = 0x06,
    WOM_RATE_31_25HZ = 0x07,
    WOM_RATE_62_50HZ = 0x08,
    WOM_RATE_125HZ = 0x09,
    WOM_RATE_250HZ = 0x0A,
    WOM_RATE_500HZ = 0x0B
  };




  #define WHOAMI_MPU9250_ 0x71
  #define WHOAMI_MPU9255_ 0x73
  #define WHOAMI_AK8963_ 0x48


	// I2C Dev Adress
  #define DEV 0x68

  /* Registers */
  #define PWR_MGMNT_1_ 0x6B
  #define H_RESET_ 0x80
  #define CLKSEL_PLL_ 0x01
  #define WHOAMI_ 0x75
	#define SIGNAL_PATH_RESET 0X68
  #define ACCEL_CONFIG_ 0x1C
  #define GYRO_CONFIG_ 0x1B
	#define GYRO_ACCEL_TEMP_RST 0x07
  #define ACCEL_CONFIG2_ 0x1D
  #define CONFIG_ 0x1A
  #define SMPLRT_DIV_ 0x19
  #define INT_PIN_CFG_ 0x37
  #define INT_ENABLE_ 0x38
  #define INT_DISABLE_ 0x00
  #define INT_PULSE_50US_ 0x00
  #define INT_RAW_RDY_EN_ 0x01
  #define INT_STATUS_ 0x3A
  #define RAW_DATA_RDY_INT_ 0x01
  #define USER_CTRL_ 0x6A
  #define I2C_MST_EN_ 0x20
  #define I2C_MST_CLK_ 0x0D
  #define I2C_MST_CTRL_ 0x24
  #define I2C_SLV0_ADDR_ 0x25
  #define I2C_SLV0_REG_ 0x26
  #define I2C_SLV0_CTRL_ 0x27
  #define I2C_SLV0_DO_ 0x63
  #define I2C_READ_FLAG_ 0x80
  #define I2C_SLV0_EN_ 0x80
  #define EXT_SENS_DATA_00_ 0x49
  
	/* Needed for WOM */
  #define INT_WOM_EN_ 0x40
  #define PWR_MGMNT_2_ 0x6C
  #define DISABLE_GYRO_ 0x07
  #define MOT_DETECT_CTRL_ 0x69
  #define ACCEL_INTEL_EN_ 0x80
  #define ACCEL_INTEL_MODE_ 0x40
  #define LP_ACCEL_ODR_ 0x1E
  #define WOM_THR_ 0x1F
  #define PWR_CYCLE_WOM_ 0x20



// Read-only Reg
#define AK8963_I2C_ADDR 0x0C // slave address for the AK8963
#define WHOAMI_AK8963_ 0x48
#define AK8963_WIA 0x00
#define AK8963_INFO 0x01
#define AK8963_ST1 0x02 // data ready status bit 0
#define AK8963_XOUT_L 0x03 // data
#define AK8963_XOUT_H 0x04
#define AK8963_YOUT_L 0x05
#define AK8963_YOUT_H 0x06
#define AK8963_ZOUT_L 0x07
#define AK8963_ZOUT_H 0x08
#define AK8963_ST2 0x09 // Data overflow bit 3 and data read error status bit 2
#define AK8963_CNTL 0x0A // Power down (0000), single-measurement (0001), self-test (1000) and Fuse ROM (1111) modes on bits 3:0
#define AK8963_ASTC 0x0C // Self test control
#define AK8963_I2CDIS 0x0F // I2C disable
#define AK8963_ASAX 0x10 // Fuse ROM x-axis sensitivity adjustment value
#define AK8963_ASAY 0x11 // Fuse ROM y-axis sensitivity adjustment value
#define AK8963_ASAZ 0x12 // Fuse ROM z-axis sensitivity adjustment value

#define AK8963_DATA_RDY_INT_ 0x01
#define AK8963_HXL_ 0x03
#define AK8963_PWR_DOWN_ 0x00
#define AK8963_CNT_MEAS1_ 0x12
#define AK8963_CNT_MEAS2_ 0x16
#define AK8963_FUSE_ROM_ 0x0F
#define AK8963_CNTL2_ 0x0B
#define AK8963_RESET_ 0x01
#define AK8963_ASA_ 0x10
#define AK8963_WHOAMI_ 0x00
#define AK8963_HOFL_ 0x08

// Write/Read Reg
#define AK8963_CNTL1 0x0A
#define AK8963_CNTL2 0x0B
#define AK8963_ASTC 0x0C
#define AK8963_TS1 0x0D
#define AK8963_TS2 0x0E
#define AK8963_I2CDIS 0x0F

// Read-only Reg ( ROM )
#define AK8963_ASAX 0x10
#define AK8963_ASAY 0x11
#define AK8963_ASAZ 0x12

// First write to register for reading
uint32_t WriteRegister(uint8_t reg); 
//  Read register
uint32_t ReadRegisters(uint8_t reg, uint8_t count, uint8_t *data);
// Write one byte to register
uint32_t WriteRegisterByte(uint8_t reg, uint8_t data);

uint32_t WriteAk8963Register(uint8_t reg, uint8_t data);

uint32_t ReadAk8963Registers(uint8_t reg,  uint8_t count,
                                  uint8_t *data);
																	
uint32_t Init_MPU9250(void);			

uint32_t ConfigAccelRange(enum AccelRange range);

uint32_t ConfigGyroRange(enum GyroRange range);

uint32_t ConfigDlpfBandwidth(enum DlpfBandwidth dlpf);

uint32_t ConfigSrd(uint8_t srd);

uint8_t Read(void); 

int ReadMagnetometerData(uint8_t *data, int16_t *data2);

//	bool new_imu_data();
//  float accel_x_mps2();
//  float accel_y_mps2(); 
//  float accel_z_mps2();
//  float gyro_x_radps();
//  float gyro_y_radps(); 
//  float gyro_z_radps(); 
//  bool new_mag_data(); 
//  float mag_x_ut(); 
//  float mag_y_ut(); 
//  float mag_z_ut(); 
//  float die_temp_c(); 

#endif

