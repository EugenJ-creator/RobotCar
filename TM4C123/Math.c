
// Implemented by Evgheni Jaruc

#include <stdint.h>
#include "Math.h"

//Reurns absolute value
int32_t absolute(int32_t value) {
  if (value < 0)
    return -value;
  return value;  
}

