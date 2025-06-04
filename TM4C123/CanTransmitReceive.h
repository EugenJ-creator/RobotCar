


#include <stdbool.h>
#include <stdint.h>


#define PART_TM4C123GH6PM

int CAN_Init(void);

int CAN_Send(uint8_t* pui8MsgDataT, uint32_t size);
