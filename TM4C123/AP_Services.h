// AP_Lab6.h
// Runs on TM4C123 or MSP432
#include <stdint.h>

// Lab 6 prototypes

typedef struct characteristics{
  uint16_t theHandle;          // each object has an ID
  uint16_t size;               // number of bytes in user data (1,2,4,8)
  uint8_t *pt;                 // pointer to user data, stored little endian
  void (*callBackRead)(void);  // action if SNP Characteristic Read Indication
  void (*callBackWrite)(void); // action if SNP Characteristic Write Indication
}characteristic_t;

extern characteristic_t CharacteristicList[];

//*************GetStatus**************
// Get status of connection, used in Lab 6
// Input:  none
// Output: status 0xAABBCCDD
// AA is GAPRole Status
// BB is Advertising Status
// CC is ATT Status
// DD is ATT method in progress
uint32_t GetStatus(void);

//*************GetVersion**************
// Get version of the SNP application running on the CC2650, used in Lab 6
// Input:  none
// Output: version
uint32_t GetVersion(void);

//*************AddService**************
// Add a service, used in Lab 6
// Inputs uuid is 0xFFF0, 0xFFF1, ...
// Output APOK if successful,
//        APFAIL if SNP failure
int AddService(uint16_t uuid);

//*************RegisterService**************
// Register a service, used in Lab 6
// Inputs none
// Output APOK if successful,
//        APFAIL if SNP failure
int RegisterService(void);

//*************AddCharacteristic**************
// Add a read, write, or read/write characteristic, used in Lab 6
//        for notify properties, call AP_AddNotifyCharacteristic 
// Inputs uuid is 0xFFF0, 0xFFF1, ...
//        thesize is the number of bytes in the user data 1,2,4, or 8 
//        pt is a pointer to the user data, stored little endian
//        permission is GATT Permission, 0=none,1=read,2=write, 3=Read+write 
//        properties is GATT Properties, 2=read,8=write,0x0A=read+write
//        name is a null-terminated string, maximum length of name is 20 bytes
//        (*ReadFunc) called before it responses with data from internal structure
//        (*WriteFunc) called after it accepts data into internal structure
// Output APOK if successful,
//        APFAIL if name is empty, more than 8 characteristics, or if SNP failure
int AddCharacteristic(uint16_t uuid, uint16_t thesize, void *pt, uint8_t permission,
  uint8_t properties, char name[], void(*ReadFunc)(void), void(*WriteFunc)(void));

//*************AddNotifyCharacteristic**************
// Add a notify characteristic, used in Lab 6
//        for read, write, or read/write characteristic, call AP_AddCharacteristic 
// Inputs uuid is 0xFFF0, 0xFFF1, ...
//        thesize is the number of bytes in the user data 1,2,4, or 8 
//        pt is a pointer to the user data, stored little endian
//        name is a null-terminated string, maximum length of name is 20 bytes
//        (*CCCDfunc) called after it accepts , changing CCCDvalue
// Output APOK if successful,
//        APFAIL if name is empty, more than 4 notify characteristics, or if SNP failure
int AddNotifyCharacteristic(uint16_t uuid, uint16_t thesize, void *pt,   
  char name[], void(*CCCDfunc)(void));


//*************StartAdvertisement**************
// Start advertisement, used in Lab 6
// Input:  none
// Output: APOK if successful,
//         APFAIL if notification not configured, or if SNP failure
int StartAdvertisement(void);
