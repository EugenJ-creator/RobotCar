// Implemented by Evgheni Jaruc

#include <stdint.h>
#include "I2C1.h"
#include "GyroAccelMag.h"
#include "UART0.h"

  /* Data */
  //static float G_MPS2_ = 9.80665f;
  //static float DEG2RAD_ = 3.14159265358979323846264338327950288f /180.0f;
  bool new_imu_data_, new_mag_data_;
  bool mag_sensor_overflow_;
  uint8_t mag_data_[8];
  uint8_t data_buf_[23];
  int16_t accel_cnts_[3], gyro_cnts_[3], temp_cnts_, mag_cnts_[3];
	uint16_t accel_cnts_u[3], gyro_cnts_u[3], temp_cnts_u, mag_cnts_u[3];
  float accel_[3], gyro_[3], mag_[3];
  float temp_;

	float accel_scale_, requested_accel_scale_;
  float gyro_scale_, requested_gyro_scale_;
  uint8_t srd_;

  //static  float TEMP_SCALE_ = 333.87f;
	
	uint8_t who_am_i_;
	uint8_t asa_buff_[3];
	float mag_scale_[3];

	
	/* Configuration */
	enum AccelRange accel_range_;
	enum AccelRange requested_accel_range_;
  enum GyroRange gyro_range_;
	enum GyroRange requested_gyro_range_;

  enum DlpfBandwidth dlpf_bandwidth_;
	enum DlpfBandwidth requested_dlpf_;
	

unsigned short rawDataShort = 0;             // 16-bit data straight from thermometer
uint32_t rawDataLong = 0;             // 16-bit data straight from thermometer




bool newMagData ;
//int16_t mag_x_ut ;
//int16_t mag_y_ut ;
//int16_t mag_z_ut ;

bool new_imu_data;

int16_t accel_x_mps2;  // temporar
int16_t accel_y_mps2;
int16_t accel_z_mps2;
//int16_t gyro_x_radps;
//int16_t gyro_y_radps;
//int16_t gyro_z_radps;

float mag_x_ut_f ;
float mag_y_ut_f ;
float mag_z_ut_f ;

uint16_t mag_x_ut_u ;
uint16_t mag_y_ut_u ;
uint16_t mag_z_ut_u ;

uint16_t accel_x_mps2_u;
uint16_t accel_y_mps2_u;
uint16_t accel_z_mps2_u;
uint16_t gyro_x_radps_u;
uint16_t gyro_y_radps_u;
uint16_t gyro_z_radps_u;


float die_temp_c ;


uint8_t whoAmI;
uint16_t AxelXY;
uint8_t AxelXY_2[2];
uint8_t x;
uint8_t y;



// delay function for testing from sysctl.c
// which delays 3*ulCount cycles
#ifdef __TI_COMPILER_VERSION__
  //Code Composer Studio Code
  void Delay(unsigned long ulCount){
  __asm (  "    subs    r0, #1\n"
      "    bne     Delay\n"
      "    bx      lr\n");
}

#else
  //Keil uVision Code
  __asm void
  Delay(unsigned long ulCount)
  {
    subs    r0, #1
    bne     Delay
    bx      lr
  }

#endif


	
	
// Write to register for reading
uint32_t WriteRegister(uint8_t reg){

	uint32_t succes;
	succes = I2C1_Send1( DEV, reg);   
	return succes;
	
}	
	
// Write one byte to register
uint32_t WriteRegisterByte(uint8_t reg, uint8_t data){

	uint32_t succes;
	succes = I2C1_Send2( DEV, reg, data);   
	return succes;
	
}	


	// Write 1 byte to regiter
uint32_t ReadRegisters(uint8_t reg, uint8_t count, uint8_t *data){

	uint32_t succes;
	succes = I2C1_Send1( DEV, reg);   
	if (succes == 0){

	for (int i = 0; i < count; i++) 
	{
		if (count == 1){
			data[i] = I2C1_Recv(DEV);
		} else if ((i == 0) && (count>1)) {
			data[i] = I2C1_RecvByteStart(DEV);
		} else if (( i != 0) && ( i< (count-1))) {
			data[i] = I2C1_RecvByte(DEV);
		} else if (( i != 0) && ( i == (count-1))) {
			data[i] = I2C1_RecvByteStop(DEV);
		}
	}
		return 0;
		} else {
			return 1;
		}
	}
	
	//  Read from Magnetometer Ak8963
	uint32_t ReadAk8963Registers(uint8_t reg,  uint8_t count,
                                  uint8_t *data) {
  if (WriteRegisterByte(I2C_SLV0_ADDR_, AK8963_I2C_ADDR | I2C_READ_FLAG_)) {
    return 1;
  }
  if (WriteRegisterByte(I2C_SLV0_REG_, reg)) {
    return 1;
  }
  if (WriteRegisterByte(I2C_SLV0_CTRL_, I2C_SLV0_EN_ | count)) {
    return 1;
  }
  Delay(41666);
  return ReadRegisters(EXT_SENS_DATA_00_, count, data);
}
	
	
	
	
//  Write to Magnetometer Ak8963	
uint32_t WriteAk8963Register(uint8_t reg, uint8_t data) {
  uint8_t ret_val;
  if (WriteRegisterByte(I2C_SLV0_ADDR_, AK8963_I2C_ADDR)) {  //  Write Magnetometr Address 0x0C to register
    return 1;
  }
	
	
  if (WriteRegisterByte(I2C_SLV0_REG_, reg)) {
    return 1;
  }
	

  if (WriteRegisterByte(I2C_SLV0_DO_, data)) {
    return 1;
  }
	
	
  if (WriteRegisterByte(I2C_SLV0_CTRL_, I2C_SLV0_EN_ | sizeof(data))) {     // Reg addr from where to begin write data
    return 1;
  }
	
	
  if (ReadAk8963Registers(reg, sizeof(ret_val), &ret_val)) {
    return 1;
  }
  if (data == ret_val) {
    return 0;
  } else {
    return 1;
  }
}	
	
	uint32_t ConfigAccelRange(enum AccelRange range) {
  
  /* Check input is valid and set requested range and scale */
  switch (range) {
    case ACCEL_RANGE_2G: {
      requested_accel_range_ = range;
      requested_accel_scale_ = 2.0f / 32767.5f;
      break;
    }
    case ACCEL_RANGE_4G: {
      requested_accel_range_ = range;
      requested_accel_scale_ = 4.0f / 32767.5f;
      break;
    }
    case ACCEL_RANGE_8G: {
      requested_accel_range_ = range;
      requested_accel_scale_ = 8.0f / 32767.5f;
      break;
    }
    case ACCEL_RANGE_16G: {
      requested_accel_range_ = range;
      requested_accel_scale_ = 16.0f / 32767.5f;
      break;
    }
    default: {
      return 1;
    }
  }
  /* Try setting the requested range */
  if (WriteRegisterByte(ACCEL_CONFIG_, requested_accel_range_)) {
    return 1;
  }
  /* Update stored range and scale */
  accel_range_ = requested_accel_range_;
  accel_scale_ = requested_accel_scale_;
  return 0;
}
	

// Config Gyro Range
uint32_t ConfigGyroRange(enum GyroRange range) {

  /* Check input is valid and set requested range and scale */
  switch (range) {
    case GYRO_RANGE_250DPS: {
      requested_gyro_range_ = range;
      requested_gyro_scale_ = 250.0f / 32767.5f;
      break;
    }
    case GYRO_RANGE_500DPS: {
      requested_gyro_range_ = range;
      requested_gyro_scale_ = 500.0f / 32767.5f;
      break;
    }
    case GYRO_RANGE_1000DPS: {
      requested_gyro_range_ = range;
      requested_gyro_scale_ = 1000.0f / 32767.5f;
      break;
    }
    case GYRO_RANGE_2000DPS: {
      requested_gyro_range_ = range;
      requested_gyro_scale_ = 2000.0f / 32767.5f;
      break;
    }
    default: {
      return 1;
    }
  }
  /* Try setting the requested range */
  if (WriteRegisterByte(GYRO_CONFIG_, requested_gyro_range_)) {
    return 1;
  }
  /* Update stored range and scale */
  gyro_range_ = requested_gyro_range_;
  gyro_scale_ = requested_gyro_scale_;
  return 0;
}

//  Config DlpfBandwith
uint32_t ConfigDlpfBandwidth(enum DlpfBandwidth dlpf) {

  /* Check input is valid and set requested dlpf */
  switch (dlpf) {
    case DLPF_BANDWIDTH_184HZ: {
      requested_dlpf_ = dlpf;
      break;
    }
    case DLPF_BANDWIDTH_92HZ: {
      requested_dlpf_ = dlpf;
      break;
    }
    case DLPF_BANDWIDTH_41HZ: {
      requested_dlpf_ = dlpf;
      break;
    }
    case DLPF_BANDWIDTH_20HZ: {
      requested_dlpf_ = dlpf;
      break;
    }
    case DLPF_BANDWIDTH_10HZ: {
      requested_dlpf_ = dlpf;
      break;
    }
    case DLPF_BANDWIDTH_5HZ: {
      requested_dlpf_ = dlpf;
      break;
    }
    default: {
      return 1;
    }
  }
  /* Try setting the dlpf */
  if (WriteRegisterByte(ACCEL_CONFIG2_, requested_dlpf_)) {
    return 1;
  }
  if (WriteRegisterByte(CONFIG_, requested_dlpf_)) {
    return 1;
  }
  /* Update stored dlpf */
  dlpf_bandwidth_ = requested_dlpf_;
  return 0;
}

// Config Srd
uint32_t ConfigSrd(uint8_t srd) {

  /* Changing the SRD to allow us to set the magnetometer successfully */
  if (WriteRegisterByte(SMPLRT_DIV_, 19)) {
    return 1;
  }
  /* Set the magnetometer sample rate */
  if (srd > 9) {
    /* Set AK8963 to power down */
    WriteAk8963Register(AK8963_CNTL1, AK8963_PWR_DOWN_);
    Delay(41666);  // long wait between AK8963 mode changes
    /* Set AK8963 to 16 bit resolution, 8 Hz update rate */
    if (WriteAk8963Register(AK8963_CNTL1, AK8963_CNT_MEAS1_)) {
      return 1;
    }
    Delay(41666);  // long wait between AK8963 mode changes
    if (ReadAk8963Registers(AK8963_ST1, sizeof(mag_data_), mag_data_)) {
      return 1;
    }
  } else {
    /* Set AK8963 to power down */
    WriteAk8963Register(AK8963_CNTL1, AK8963_PWR_DOWN_);
    Delay(41666);  // long wait between AK8963 mode changes
    /* Set AK8963 to 16 bit resolution, 100 Hz update rate */
    if (WriteAk8963Register(AK8963_CNTL1, AK8963_CNT_MEAS2_)) {
      return 1;
    }
    Delay(41666);  // long wait between AK8963 mode changes
    if (ReadAk8963Registers(AK8963_ST1, sizeof(mag_data_), mag_data_)) {
      return 1;
    }
  }
  /* Set the IMU sample rate */
  if (WriteRegisterByte(SMPLRT_DIV_, srd)) {
    return 1;
  }
  srd_ = srd;
  return 0;
}

// Read updated data  
uint8_t Read(void) {

  /* Reset the new data flags */
  new_mag_data_ = FALSE;
  new_imu_data_ = FALSE;
  /* Read the data registers */
  if (ReadRegisters(INT_STATUS_, sizeof(data_buf_), data_buf_)) {
    return 1;
  }
  /* Check if data is ready */
  new_imu_data_ = (data_buf_[0] & RAW_DATA_RDY_INT_);
  if (!new_imu_data_) {
    return 1;
  }
	
//	
//	I2C_Send1( 0x3C, INT_STATUS_);
//	I2C_Recv6 (DEV, data_buf_);
	
	
	
	
  /* Unpack the buffer and transform to sighed integer*/
  accel_cnts_[0] = (uint16_t)(data_buf_[1])  << 8 | data_buf_[2];
  accel_cnts_[1] = (uint16_t)(data_buf_[3])  << 8 | data_buf_[4];
  accel_cnts_[2] = (uint16_t)(data_buf_[5])  << 8 | data_buf_[6];
  temp_cnts_ =     (uint16_t)(data_buf_[7])  << 8 | data_buf_[8];
  gyro_cnts_[0] =  (uint16_t)(data_buf_[9])  << 8 | data_buf_[10];
  gyro_cnts_[1] =  (uint16_t)(data_buf_[11]) << 8 | data_buf_[12];
  gyro_cnts_[2] =  (uint16_t)(data_buf_[13]) << 8 | data_buf_[14];
  new_mag_data_ = (data_buf_[15] & AK8963_DATA_RDY_INT_);
  
	mag_cnts_[0] =   (uint16_t)(data_buf_[17]) << 8 | data_buf_[16];
  mag_cnts_[1] =   (uint16_t)(data_buf_[19]) << 8 | data_buf_[18];
  mag_cnts_[2] =   (uint16_t)(data_buf_[21]) << 8 | data_buf_[20];
	
	
	
//	  /* Unpack the buffer and transmit as unsigned for COM Port*/
  accel_cnts_u[0] = (uint16_t)(data_buf_[1])  << 8 | data_buf_[2];
  accel_cnts_u[1] = (uint16_t)(data_buf_[3])  << 8 | data_buf_[4];
  accel_cnts_u[2] = (uint16_t)(data_buf_[5])  << 8 | data_buf_[6];
  temp_cnts_u =     (uint16_t)(data_buf_[7])  << 8 | data_buf_[8];
  gyro_cnts_u[0] =  (uint16_t)(data_buf_[9])  << 8 | data_buf_[10];
  gyro_cnts_u[1] =  (uint16_t)(data_buf_[11]) << 8 | data_buf_[12];
  gyro_cnts_u[2] =  (uint16_t)(data_buf_[13]) << 8 | data_buf_[14];
  new_mag_data_ = (data_buf_[15] & AK8963_DATA_RDY_INT_);
  mag_cnts_u[0] =   (uint16_t)(data_buf_[17]) << 8 | data_buf_[16];
  mag_cnts_u[1] =   (uint16_t)(data_buf_[19]) << 8 | data_buf_[18];
  mag_cnts_u[2] =   (uint16_t)(data_buf_[21]) << 8 | data_buf_[20];
	
	
	
	
	
	
	
	
	
	
  /* Check for mag overflow */
  mag_sensor_overflow_ = (data_buf_[22] & AK8963_HOFL_);
  if (mag_sensor_overflow_) {
    new_mag_data_ = FALSE;
  }
//  /* Convert to float values and rotate the accel / gyro axis */
//  accel_[0] = (uint16_t)(accel_cnts_[1]) * accel_scale_ * G_MPS2_;
//  accel_[1] = (uint16_t)(accel_cnts_[0]) * accel_scale_ * G_MPS2_;
//  accel_[2] = (uint16_t)(accel_cnts_[2]) * accel_scale_ * -1.0f *
//              G_MPS2_;
//  temp_ = ((uint16_t)(temp_cnts_) - 21.0f) / TEMP_SCALE_ + 21.0f;
//  gyro_[0] = (uint16_t)(gyro_cnts_[1]) * gyro_scale_ * DEG2RAD_;
//  gyro_[1] = (uint16_t)(gyro_cnts_[0]) * gyro_scale_ * DEG2RAD_;
//  gyro_[2] = (uint16_t)(gyro_cnts_[2]) * gyro_scale_ * -1.0f * DEG2RAD_;
	
/////---------------------------------------------------------Scale if need---------------	
	
//	  /* Convert to float values and not rotate the accel / gyro axis */
//  accel_[0] = (uint16_t)(accel_cnts_[0]) * accel_scale_ * G_MPS2_;
//  accel_[1] = (uint16_t)(accel_cnts_[1]) * accel_scale_ * G_MPS2_;
//  accel_[2] = (uint16_t)(accel_cnts_[2]) * accel_scale_ * -1.0f *
//              G_MPS2_;
//  temp_ = ((uint16_t)(temp_cnts_) - 21.0f) / TEMP_SCALE_ + 21.0f;
//  gyro_[0] = (uint16_t)(gyro_cnts_[0]) * gyro_scale_ * DEG2RAD_;
//  gyro_[1] = (uint16_t)(gyro_cnts_[1]) * gyro_scale_ * DEG2RAD_;
//  gyro_[2] = (uint16_t)(gyro_cnts_[2]) * gyro_scale_ * -1.0f * DEG2RAD_;
	
	
	
//  /* Only update on new data */
//  if (new_mag_data_) {
//    mag_[0] =   (uint16_t)(mag_cnts_[0]) * mag_scale_[0];
//    mag_[1] =   (uint16_t)(mag_cnts_[1]) * mag_scale_[1];
//    mag_[2] =   (uint16_t)(mag_cnts_[2]) * mag_scale_[2];
//  }
  return 0;
}

///---------------------------------------------------------------------------------------------
//	bool new_imu_data()  {return new_imu_data_;}
//  float accel_x_mps2()  {return accel_[0];}
//  float accel_y_mps2()  {return accel_[1];}
//  float accel_z_mps2()  {return accel_[2];}
//  float gyro_x_radps()  {return gyro_[0];}
//  float gyro_y_radps()  {return gyro_[1];}
//  float gyro_z_radps()  {return gyro_[2];}
//  bool new_mag_data()  {return new_mag_data_;}
//  float mag_x_ut()  {return mag_[0];}
//  float mag_y_ut()  {return mag_[1];}
//  float mag_z_ut()  {return mag_[2];}
//  float die_temp_c()  {return temp_;}

uint32_t Init_MPU9250(void) {
//  I2C_Send1(0x38, 1);                     // use command 1 to set pointer to config (Figure 7.XX chapter7-10-1.ulb)
//                                          // read from 0x48 to get data
//  if(I2C_Recv2(0x48) == 0x60A0){          // expected 0x60A0 as power-on default (Figure 7.XX chapter7-10-2.ulb)
//    UART_OutString("\r\nTest Passed\r\n");
//  }
//  else{
//    if(I2C3_MASTER_MCS_R&0x02){
//      UART_OutString("\r\nNo Acknowledge\r\n");
//    }
//    else{
//      UART_OutString("\r\nTest Failed\r\n");
//    }
//  }
	
	
//	I2C_Send1(0x38, 0xBA); //Soft Reset
//	Delay(41666); 

	///* Set AK8963 to power down . Don't apply   */
//  WriteAk8963Register(AK8963_CNTL1, AK8963_PWR_DOWN_);
//  /* Reset the MPU9250 */
  WriteRegisterByte(PWR_MGMNT_1_, H_RESET_);
  /* Wait for MPU-9250 to come back up */
  Delay(41666); 
  /* Reset the AK8963 */
  WriteAk8963Register(AK8963_CNTL2_, AK8963_RESET_);
	 
	Delay(41666); 
  //---------------------------------

	
///////// From ARduino. Not necessary to init. Is working fine without	
	
  /* Select clock source to gyro */
  if (WriteRegisterByte(PWR_MGMNT_1_, CLKSEL_PLL_)) {
    return 1;
  }
  /* Enable I2C master mode */
  if (WriteRegisterByte(USER_CTRL_, I2C_MST_EN_)) {
    return 1;
  }
  /* Set the I2C bus speed to 400 kHz */
  if (WriteRegisterByte(I2C_MST_CTRL_, I2C_MST_CLK_)) {
    return 1;
  }
	
	
//	I2C_Send2(0x68, 0x6A, 0x10);   //  0x10  1B   DisableI2C
	
	
	WriteRegisterByte(SIGNAL_PATH_RESET, GYRO_ACCEL_TEMP_RST);  //   Resete acc, gyro and temp
	
	Delay(41666); 



///* Select clock source to gyro */
//  if (!WriteRegister(PWR_MGMNT_1_, CLKSEL_PLL_)) {
//    return false;
//  }
  /* Check the WHO AM I byte */
  if (ReadRegisters(WHOAMI_, sizeof(who_am_i_), &who_am_i_)) {  // If result is 1 then failed. Exit function
    return 1;
  }
  if (who_am_i_ != WHOAMI_MPU9255_) {
    return 1;
  }
	
	
//  /* Enable I2C master mode */
//  if (!WriteRegisterByte(USER_CTRL_, I2C_MST_EN_)) {
//    return 1;
//  }
//  /* Set the I2C bus speed to 400 kHz */
//  if (!WriteRegisterByte(I2C_MST_CTRL_, I2C_MST_CLK_)) {
//    return 1;
//  }

  /* Check the AK8963 WHOAMI */
  if (ReadAk8963Registers(AK8963_WHOAMI_, sizeof(who_am_i_), &who_am_i_)) {
    return 1;
  }
  if (who_am_i_ != WHOAMI_AK8963_) {
    return 1;
  }
  /* Get the magnetometer calibration */
  /* Set AK8963 to power down */
  if (WriteAk8963Register(AK8963_CNTL1, AK8963_PWR_DOWN_)) {
    return 1;
  }
  Delay(41666);  // long wait between AK8963 mode changes
	///////////////////////////////////////////////////////////////////////////////////////////////////////To DO!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!
  /* Set AK8963 to FUSE ROM access */
  if (WriteAk8963Register(AK8963_CNTL1, AK8963_FUSE_ROM_)) {
    return 1;
  }
  Delay(41666);  // long wait between AK8963 mode changes
  /* Read the AK8963 ASA registers and compute magnetometer scale factors */
  if (ReadAk8963Registers(AK8963_ASA_, sizeof(asa_buff_), asa_buff_)) {
    return 1;
  }
  mag_scale_[0] = (((float)(asa_buff_[0]) - 128.0f)
    / 256.0f + 1.0f) * 4912.0f / 32760.0f;
  mag_scale_[1] = (((float)(asa_buff_[1]) - 128.0f)
    / 256.0f + 1.0f) * 4912.0f / 32760.0f;
  mag_scale_[2] = (((float)(asa_buff_[2]) - 128.0f)
    / 256.0f + 1.0f) * 4912.0f / 32760.0f;
  /* Set AK8963 to power down */
  if (WriteAk8963Register(AK8963_CNTL1, AK8963_PWR_DOWN_)) {
    return 1;
  }
////////////////////////////////////////////////////////////////////////////////////////////////////////TO DO!!!!!!!!!!!!!!!
  /* Set AK8963 to 16 bit resolution, 100 Hz update rate */
  if (WriteAk8963Register(AK8963_CNTL1, AK8963_CNT_MEAS2_)) {
    return 1;
  }
  Delay(41666);  // long wait between AK8963 mode changes
  /* Select clock source to gyro */
  if (WriteRegisterByte(PWR_MGMNT_1_, CLKSEL_PLL_)) {
    return 1;
  }
  /* Set the accel range to 16G by default */
  if (ConfigAccelRange(ACCEL_RANGE_16G)) {
    return 1;
  }
  /* Set the gyro range to 2000DPS by default*/
  if (ConfigGyroRange(GYRO_RANGE_2000DPS)) {
    return 1;
  }
  /* Set the DLPF to 184HZ by default */
  if (ConfigDlpfBandwidth(DLPF_BANDWIDTH_184HZ)) {
    return 1;
  }
	 Delay(41666);  // long wait between AK8963 mode changes
  /* Set the SRD to 0 by default */
  if (ConfigSrd(0)) {
    return 1;
  }
  return 0;
}



int ReadMagnetometerData(uint8_t *data, int16_t *data2){


		
/* Check if data read */
  if (Read()==0) {


 
//  To Check Int Values of Magnitometer , Signed Int
		data2[0] =  (int16_t)mag_cnts_[0];
		data2[1] =  (int16_t)mag_cnts_[1];
		data2[2] =  (int16_t)mag_cnts_[2];

		data[0] =  (uint8_t)data_buf_[21];
		data[1] =  (uint8_t)data_buf_[20];
		data[2] =  (uint8_t)data_buf_[19];
		data[3] =  (uint8_t)data_buf_[18];
		data[4] =  (uint8_t)data_buf_[17];
		data[5] =  (uint8_t)data_buf_[16];


	////------------------------------------------------------------		

	
//---------------------------- For calibration---------------------
	
//  mag_x_ut_u = mag_cnts_u[0];
//  mag_y_ut_u =  mag_cnts_u[1];
//  mag_z_ut_u =  mag_cnts_u[2];
//	
//	accel_x_mps2_u = accel_cnts_u[0];
//	accel_y_mps2_u = accel_cnts_u[1];
//	accel_z_mps2_u = accel_cnts_u[2];

//	gyro_x_radps_u = gyro_cnts_u[0];
//	gyro_y_radps_u = gyro_cnts_u[1];
//	gyro_z_radps_u = gyro_cnts_u[2];
//	
//	
//	
//	UART0_OutUHex(accel_x_mps2_u);	
//	UART0_OutChar(';');
//	UART0_OutUHex(accel_y_mps2_u);
//	UART0_OutChar(';');
//	UART0_OutUHex(accel_z_mps2_u);
//	UART0_OutChar(';');	
//	

//	UART0_OutUHex(mag_x_ut_u);	
//	UART0_OutChar(';');
//	UART0_OutUHex(mag_y_ut_u);
//	UART0_OutChar(';');
//	UART0_OutUHex(mag_z_ut_u);	
//	UART0_OutChar(';');
//	


//	UART0_OutUHex(gyro_x_radps_u);	
//	UART0_OutChar(';');
//	UART0_OutUHex(gyro_y_radps_u);
//	UART0_OutChar(';');
//	UART0_OutUHex(gyro_z_radps_u);	
//		
//	UART0_OutString(";\n");
	
//------------------------------	
	
	
	
	
	
	
	
	
	return 0;
	}	else{
		return 1;
	}

}
