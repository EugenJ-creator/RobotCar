//==================================================================================//

#include <CAN.h>


#define TX_GPIO_NUM   21  // Connects to CTX
#define RX_GPIO_NUM   22  // Connects to CRX

#define LED_PIN_DAY     19 // ESP32 pin Day

#define LED_PIN_LEFT    18 // ESP32 pin Left 
#define LED_PIN_RIGHT     17 // ESP32 pin Right

#define LED_PIN_REAR     16 // ESP32 pin REAR

#define LED_PIN_EN     13 // Enable Pin for Driver
//==================================================================================//
void LedBlink(int led, int status);  // 3-left, 4 - right, 5 both
void LedBackwardsDC(int status);
void LedDayLight(int status);

// the number of the LED pin
const int ledPin = 23;  // 23 corresponds to GPIO 23

TaskHandle_t taskCANReceiveHandle; // create task handle
uint leftActive = 0;
uint rightActive = 0;
uint warningActive = 0;
hw_timer_t *timerLeft = NULL;
hw_timer_t *timerRight = NULL;
hw_timer_t *timerWarning = NULL;

// ISR for Timer0
void IRAM_ATTR toggle_left_led(){
  if (warningActive == 0){
    digitalWrite(LED_PIN_LEFT, !digitalRead(LED_PIN_LEFT)); // toggle the LED
  }
}

void IRAM_ATTR toggle_right_led() {
  if (warningActive == 0){
    digitalWrite(LED_PIN_RIGHT, !digitalRead(LED_PIN_RIGHT)); // toggle the LED
  }
}

void IRAM_ATTR toggle_warning_led() {
  digitalWrite(LED_PIN_RIGHT, !digitalRead(LED_PIN_RIGHT)); // toggle the LED warning
  digitalWrite(LED_PIN_LEFT, !digitalRead(LED_PIN_LEFT)); // 
}



void setup() {
 

  Serial.begin (115200);
  while (!Serial);
  delay (1000);

  Serial.println ("CAN Receiver/Receiver");

  // Set the pins
  CAN.setPins (RX_GPIO_NUM, TX_GPIO_NUM);

  // start the CAN bus at 500 kbps
  if (!CAN.begin (500E3)) {
    Serial.println ("Starting CAN failed!");
    while (1);
  }
  else {
    Serial.println ("CAN Initialized");
  }
  pinMode(LED_PIN_DAY, OUTPUT);
  pinMode(LED_PIN_LEFT, OUTPUT);
  pinMode(LED_PIN_RIGHT, OUTPUT);
  pinMode(LED_PIN_REAR, OUTPUT);

  // set the LED as an output
  pinMode(ledPin, OUTPUT);

  pinMode(LED_PIN_EN, OUTPUT);
  // set the LED with the ledState of the variable:
  digitalWrite(LED_PIN_DAY, HIGH);
  digitalWrite(LED_PIN_LEFT, HIGH);
  digitalWrite(LED_PIN_RIGHT, HIGH);
  digitalWrite(LED_PIN_REAR, HIGH);

  // set the LED with the ledState of the variable:
  digitalWrite(LED_PIN_EN, HIGH);


  //Input Params: The first one is the number of the timer we want to use (from 0 to 3, since we have 4 hardware timers), 
  //the second one is the value of the prescaler and the last one is a flag indicating if the counter should count up (true) or down (false). 
  //In this example, we are using timer 0 with the prescaler 80 and the counter to count up.
  timerLeft = timerBegin(300000);

  // Before enabling the timer, we need to attach it to an ISR, which will be executed when the interrupt is generated. 
  //This is done with a call to the timerAttachInterrupt function. In this example, 
  //we have attached the ISR function called onTimer to the timer interrupt.
  timerAttachInterrupt(timerLeft, &toggle_left_led);

  // timerAlarmWrite function is used to specify the counter value in which the timer interrupt should be generated. 
  //So, for this example, we assume that we want to generate an interrupt each second, 
  //and thus we pass the value of 500000 microseconds, which is equal to 0.5 second. 
  //For the third argument, we will pass the value true, so the counter will reload and thus the interrupt will be generated periodically.
  timerAlarm(timerLeft, 300000, true, 0);
  

//---------------------------------------------------------
//Input Params: The first one is the number of the timer we want to use (from 0 to 3, since we have 4 hardware timers), 
  //the second one is the value of the prescaler and the last one is a flag indicating if the counter should count up (true) or down (false). 
  //In this example, we are using timer 0 with the prescaler 80 and the counter to count up.
  timerRight = timerBegin(300000);

  // Before enabling the timer, we need to attach it to an ISR, which will be executed when the interrupt is generated. 
  //This is done with a call to the timerAttachInterrupt function. In this example, 
  //we have attached the ISR function called onTimer to the timer interrupt.
  timerAttachInterrupt(timerRight, &toggle_right_led);

  // timerAlarmWrite function is used to specify the counter value in which the timer interrupt should be generated. 
  //So, for this example, we assume that we want to generate an interrupt each second, 
  //and thus we pass the value of 500000 microseconds, which is equal to 0.5 second. 
  //For the third argument, we will pass the value true, so the counter will reload and thus the interrupt will be generated periodically.
  timerAlarm(timerRight, 300000, true, 0);

  //----------------------------------------------------------
  //Input Params: The first one is the number of the timer we want to use (from 0 to 3, since we have 4 hardware timers), 
  //the second one is the value of the prescaler and the last one is a flag indicating if the counter should count up (true) or down (false). 
  //In this example, we are using timer 0 with the prescaler 80 and the counter to count up.
  timerWarning = timerBegin(300000);

  // Before enabling the timer, we need to attach it to an ISR, which will be executed when the interrupt is generated. 
  //This is done with a call to the timerAttachInterrupt function. In this example, 
  //we have attached the ISR function called onTimer to the timer interrupt.
  timerAttachInterrupt(timerWarning, &toggle_warning_led);

  // timerAlarmWrite function is used to specify the counter value in which the timer interrupt should be generated. 
  //So, for this example, we assume that we want to generate an interrupt each second, 
  //and thus we pass the value of 500000 microseconds, which is equal to 0.5 second. 
  //For the third argument, we will pass the value true, so the counter will reload and thus the interrupt will be generated periodically.
  timerAlarm(timerWarning, 300000, true, 0);
  
  timerStop(timerRight);
  timerStop(timerLeft);
  timerStop(timerWarning);




xTaskCreate(
        canReceiver,      // Function name of the task
        "CAN Receiver",   // Name of the task (e.g. for debugging)
        2048,        // Stack size (bytes)
        NULL,        // Parameter to pass
        2,           // Task priority
        &taskCANReceiveHandle   // Assign task handle
      );



}





void canReceiver(void *pvParameters){

    while(1)
    {
      // try to parse packet
      int packetSize = CAN.parsePacket();

      if (packetSize) {
        uint8_t pui8MsgDataR[packetSize];    // Create empty byte array
        // received a packet
        Serial.print ("Received ");

        if (CAN.packetExtended()) {
          Serial.print ("extended ");
        }

        if (CAN.packetRtr()) {
          // Remote transmission request, packet contains no data
          Serial.print ("RTR ");
        }

        Serial.print ("packet with id 0x");
        Serial.print (CAN.packetId(), HEX);

        if (CAN.packetRtr()) {
          Serial.print (" and requested length ");
          Serial.println (CAN.packetDlc());
        } else {
          Serial.print (" and length ");
          Serial.println (packetSize);

          // only print packet data for non-RTR packets
          while (CAN.available()) {
            
            //Serial.println (CAN.read());


            for (int i=0; i<packetSize; i++){
              pui8MsgDataR[i] = CAN.read();
              Serial.print ((int) pui8MsgDataR[i]);
            }
          }
          Serial.println();

          switch (pui8MsgDataR[0]) {
            case 1:
                if (pui8MsgDataR[1] == 0x2)    //    Rear Light   
                {
                  LedBackwardsDC(pui8MsgDataR[2]);
                }
                break;

            case 2:
                LedBlink(pui8MsgDataR[1], pui8MsgDataR[2]);  // Blinking Leds
                LedBlink(pui8MsgDataR[3], pui8MsgDataR[4]);
                break;
            case 3:
                if (pui8MsgDataR[1] == 0x2)    //    Rear Light   
                {
                  LedDayLight(pui8MsgDataR[2]);
                }
                break;
            case 4:
                if ((pui8MsgDataR[1] == 5) && (pui8MsgDataR[2] == 1)){
                  warningActive = 1;
                } else if ((pui8MsgDataR[1] == 5) && (pui8MsgDataR[2] == 0)){
                  warningActive = 0;
                }
                LedBlink(pui8MsgDataR[1], pui8MsgDataR[2]);  //Warning  Blinking Leds
                break;  
            case 5: 
                LedPWM(pui8MsgDataR[1], pui8MsgDataR[2]);    // Set PWM Duty cycle if PWM is enabled
                break;
          }
        Serial.println();
        } 
      }
    }


}
//==================================================================================//

void loop() {
  //canSender();
  //canReceiver();

//if (rightActive == 1){
  //digitalWrite(LED_PIN_REAR, LOW);
//} else if (rightActive == 0){
  //digitalWrite(LED_PIN_REAR, LOW);
//}


// timerStop(timerRight);
// digitalWrite(LED_PIN_RIGHT, HIGH); // toggle the LED
// timerStart(timerLeft);

// delay(25000);

// timerStop(timerLeft);
// digitalWrite(LED_PIN_LEFT, HIGH); // toggle the LED
// timerStart(timerRight);

// delay(25000);
}

//==================================================================================//

void canSender() {
  // send packet: id is 11 bits, packet can contain up to 8 bytes of data
  Serial.print ("Sending packet ... ");

  CAN.beginPacket (0xA,4);  //sets the ID and clears the transmit buffer
  // CAN.beginExtendedPacket(0xabcdef);
  CAN.write (0xAA); //write data to buffer. data is not sent until endPacket() is called.
  CAN.write (0xAA);
  CAN.write (0xAA);
  CAN.write (0xAA);
  //CAN.write ('5');
  //CAN.write ('6');
  //CAN.write ('7');
  //CAN.write ('8');
  CAN.endPacket();

  //RTR packet with a requested data length
  //CAN.beginPacket (0x12, 3, true);
  //CAN.endPacket();

  Serial.println ("done");

  delay (1000);
}

//==================================================================================//
void LedPWM(int Enable, int dutyCycle ){
  Serial.println ("En DC:");
  Serial.print (Enable);
  Serial.print ("   ");
  Serial.print (dutyCycle);

  if (Enable){
    
    digitalWrite(LED_PIN_EN, LOW);
    analogWrite(ledPin, dutyCycle);
    //ledcWrite(PWM_CHANNEL, dutyCycle);
  } else {
    digitalWrite(LED_PIN_EN, HIGH);
    analogWrite(ledPin, 0);
    //ledcWrite(PWM_CHANNEL, 0);
  }
  
}


void LedBackwardsDC(int status){
  if (status == 1){
    digitalWrite(LED_PIN_REAR, LOW);
  } else  if (status == 0){
    digitalWrite(LED_PIN_REAR, HIGH);
  }
}

void LedDayLight(int status){
  if (status == 1){
    digitalWrite(LED_PIN_DAY, LOW);
  } else  if (status == 0){
    digitalWrite(LED_PIN_DAY, HIGH);
  }
}


void LedBlink(int led, int status){
              
          if (led== 3) {        // Left on
            if (status == 1){
              // Enable timer
              timerStart(timerLeft);
            } else  if (status == 0){        // Left off
              //leftActive = 0;
              // Stop and free timer
              timerStop(timerLeft);
              digitalWrite(LED_PIN_LEFT, HIGH); // toggle the LED
            }
          }  else if (led== 4){        // Right on
            if (status == 1){
              //rightActive = 1;
              // Enable timer
              timerStart(timerRight);
              
            } else  if (status == 0){       // Right off
              //rightActive = 0;
              timerStop(timerRight);
              digitalWrite(LED_PIN_RIGHT, HIGH); // toggle the LED
              
            }
          } else if (led == 5){        // Warning on
            if (status == 1){
              //rightActive = 1;
              // Enable timer
              timerStart(timerWarning);
              
            } else  if (status == 0){       // Warning off
              //rightActive = 0;
              timerStop(timerWarning);
              digitalWrite(LED_PIN_RIGHT, HIGH); // off the LED
              digitalWrite(LED_PIN_LEFT, HIGH); // off the LED
            }
          } 
}

//==================================================================================//