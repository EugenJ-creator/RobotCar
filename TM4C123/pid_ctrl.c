
//#include "pid_ctrl.h"
// 

// 
//void pidctl_reset(pidctl_t PID, pidctl_n_t E) {  
//    PID.i = 0;                
//    PID.e_last = E;         
//}



///**
// * Calculates the new output value (saved in O) dependent on the
// * actual error (E) and the controller configuration/state (PID).
// */
//void pidctl(pidctl_t PID, pidctl_n_t E , pidctl_n_t O) {                     
//  PID.i += PID.Ki * E;                    
//  if(PID.i > PID.i_saturation) {           
//    PID.i = PID.i_saturation;               
//  } else if(PID.i < -PID.i_saturation) {    
//    PID.i = -PID.i_saturation;              
//  }                                             
//  O = E - PID.e_last;                     
//  /* stfwi: removed D input filter  */          
//  O *= PID.Kd;                              
//  O += PID.Kp * E;                        
//  O += PID.i;                               
//  O += PID.offset;                          
//  if(O > PID.saturation) {                  
//    O = PID.saturation;                     
//  } else if(O < -PID.saturation) {          
//    O = -PID.saturation;                    
//  }                                             
//    PID.e_last = E;                           
//}


// 

